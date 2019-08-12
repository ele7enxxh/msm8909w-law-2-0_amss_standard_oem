#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/sbl1/sbl1.s"
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                              SBL1
;
; GENERAL DESCRIPTION
;   This file bootstraps the processor. The Start-up Primary Bootloader
;   (SBL1) performs the following functions:
;
;      - Set up the hardware to continue boot process.
;      - Initialize DDR memory
;      - Load Trust-Zone OS
;      - Load RPM firmware
;      - Load APPSBL and continue boot process
;
;   The SBL1 is written to perform the above functions with optimal speed.
;   It also attempts to minimize the execution time and hence reduce boot time.
;
; Copyright 2014-2015 by Qualcomm Technologies, Incorporated.All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           EDIT HISTORY FOR FILE
;
; This section contains comments describing changes made to the module.
; Notice that changes are listed in reverse chronological order.
;
; $Header: 
;
; when       who     what, where, why
; --------   ---     --------------------------------------------------------
; 11/17/14   sk      added sbl_memcpy function with neon instructions
; 02/27/15   aus     Fixed to check for correct abort mode
; 07/22/14   sk      Changes for 8909
; 07/14/14   sk      Added sbl1_external_abort_enable funtion
; 07/01/14   sk      Added sbl_save_regs function
; 05/01/14   ck      Added logic to assign stacks based on processor mode
; 03/19/14   ck      Fixed stack base issue.  Now using proper address which is "Limit" of SBL1_STACK 
; 03/07/14   ck      Removed -4 logic from check_for_nesting as bear SBL has its own vector table
; 03/03/14   ck      Updated vector table with branches as VBAR is being used in Bear family 
; 11/15/12   dh      Add boot_read_l2esr for 8974
; 08/31/12   dh      Correct the stack base in check_for_nesting, remove unused code
; 07/16/12   dh      Remove watchdog reset code
; 02/06/12   dh      Update start up code
; 01/31/12   dh      Initial revision for 8974
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


;============================================================================
;
;                            MODULE INCLUDES
;
;============================================================================
#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/src/boot_msm.h"














 

















 






 
#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"




 
















 


























 

#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/hwio/msm8909/msmhwiobase.h"




 



 


























 



 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 






#line 52 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"


 




#line 72 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 92 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 112 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 130 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"



 




#line 151 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 159 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 169 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 183 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 197 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 211 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 221 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 229 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 243 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"



 




#line 288 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 302 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 316 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 330 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 374 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 404 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 436 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 480 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 512 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 540 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 554 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 582 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 596 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 610 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 624 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 666 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 708 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 742 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 756 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 794 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 808 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 822 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 836 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 880 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 910 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 942 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 986 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1018 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1046 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1060 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1098 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1112 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1126 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1140 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1184 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1214 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1246 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1290 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1322 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1350 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1364 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1378 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1398 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1414 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1430 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1450 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1466 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1482 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1498 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1508 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1524 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1540 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1554 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1570 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1590 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1606 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1622 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1642 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1656 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1684 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1704 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1720 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1746 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1766 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1784 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1804 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1818 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1834 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1854 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1874 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1890 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1916 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1932 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1952 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1968 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 1988 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2008 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2024 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2040 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2060 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2076 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2092 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2108 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2124 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2140 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2156 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2172 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2186 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2212 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2230 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2244 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2258 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2272 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2298 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2330 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2346 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2362 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2376 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2402 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2432 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2452 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2468 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2488 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2504 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2518 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2532 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2546 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2560 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2576 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2590 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2616 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2634 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2648 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2662 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2676 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2702 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2732 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2746 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2760 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2786 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2804 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2818 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2832 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2846 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2872 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2902 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2916 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2926 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2954 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2968 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 2984 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3000 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3020 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3036 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3056 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3072 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3092 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3108 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3128 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3144 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3164 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3180 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3200 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3216 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3242 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3260 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3274 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3288 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3302 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3316 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3332 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3348 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3374 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3392 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3406 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3420 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3434 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3448 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3464 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3480 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3506 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3524 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3538 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3552 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3566 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3580 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3596 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3612 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3638 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3656 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3670 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3684 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3698 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3712 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3728 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3744 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3770 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3788 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3802 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3816 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3830 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3844 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3860 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3876 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3902 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3920 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3934 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3948 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3962 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3976 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 3992 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4008 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4034 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4052 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4066 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4080 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4094 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4108 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4124 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4140 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4166 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4184 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4198 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4212 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4226 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4246 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4260 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4280 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4294 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4314 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4332 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4348 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4368 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4384 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4398 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4416 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4430 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4450 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4464 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4492 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4506 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4534 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4548 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4566 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4576 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4590 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4608 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4626 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4636 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4664 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4680 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4698 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4718 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4734 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4750 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4764 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4784 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4800 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4814 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4840 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4860 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4886 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4902 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4918 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4938 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4954 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4968 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 4988 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5004 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5030 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5048 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5068 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5084 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5110 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5124 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5144 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5160 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5176 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5192 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5208 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5224 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5240 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5256 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5270 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5290 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5306 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5330 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5340 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5358 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5374 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5390 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5406 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5422 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5438 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5452 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5498 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5514 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5530 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5550 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5566 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5586 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5606 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5622 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5644 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5660 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5670 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5686 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5702 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5722 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5738 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5758 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5774 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5790 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5800 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5818 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5838 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5852 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5872 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5888 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5904 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5922 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5932 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5946 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5962 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5976 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 5992 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6006 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6022 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6036 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6052 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6066 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6082 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6096 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6112 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6126 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6142 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6156 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6172 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6186 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6202 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6216 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6232 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6246 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6262 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6276 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6306 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6326 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6350 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6374 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6398 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6422 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6446 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6470 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6494 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6518 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6542 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6566 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6590 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6608 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6632 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6652 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6662 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6678 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6692 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6706 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6720 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6734 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6748 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6762 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6776 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6790 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6804 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6818 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6832 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6846 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6860 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6874 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6888 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6902 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6916 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6932 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6952 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6972 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 6988 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7008 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7054 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7092 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7138 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7176 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7196 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7242 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7280 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7326 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7364 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7384 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7430 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7468 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7514 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7552 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7572 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7618 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7656 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7702 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7740 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7760 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7806 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7852 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7872 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7918 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7964 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 7984 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8030 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8044 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8058 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8072 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8104 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8118 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8134 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8156 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8170 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8184 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8210 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8226 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8238 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8272 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8288 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8298 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8314 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8340 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8358 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8372 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8386 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8400 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8416 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8442 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8460 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8474 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8488 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8502 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8518 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8544 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8562 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8576 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8590 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8604 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8624 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8638 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8652 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8666 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8680 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8694 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8708 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8722 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8736 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8752 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8768 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8784 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8800 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8816 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8830 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8846 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8862 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8878 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8894 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8910 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8926 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8942 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8968 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 8986 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9000 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9014 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9028 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9042 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9086 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9130 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9156 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9182 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9202 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9228 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9254 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9272 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9286 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9300 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9314 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9334 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9350 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9370 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9386 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9406 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9422 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9442 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9458 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9472 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9516 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9536 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9562 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9578 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9604 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9620 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9636 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9652 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9672 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9688 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9702 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9718 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9738 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9754 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9768 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9784 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9800 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9814 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9830 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9844 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9860 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9874 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9890 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9910 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9926 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9940 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9956 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9972 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 9986 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10002 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10016 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10032 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10046 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10062 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10076 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10092 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10118 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10136 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10150 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10164 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10178 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10192 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10208 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10234 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10252 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10266 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10280 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10294 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10308 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10324 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10350 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10368 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10382 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10396 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10410 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10424 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10440 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10466 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10484 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10498 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10512 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10526 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10540 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10556 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10570 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10586 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10600 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10620 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10640 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10656 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10670 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10714 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10740 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10756 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10782 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10796 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10822 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10842 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10858 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10872 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10916 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10942 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10962 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 10988 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11006 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11020 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11034 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11048 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11068 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11088 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11114 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11132 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11146 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11160 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11174 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11192 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11218 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11236 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11250 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11264 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11278 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11296 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11322 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11340 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11366 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11384 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11398 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11412 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11426 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11454 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11472 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11490 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11508 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11526 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11552 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11570 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11584 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11598 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11612 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11630 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11656 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11674 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11688 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11702 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11716 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11734 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11760 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11778 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11792 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11806 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11820 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11838 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11864 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11882 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11896 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11910 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11924 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11942 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11956 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"



 




#line 11977 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 11991 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"



 




#line 12008 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"



 




#line 12029 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12043 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12059 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12073 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12093 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12107 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12121 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12137 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12153 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12173 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12187 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12201 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12215 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12229 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12243 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12257 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12271 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12285 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12299 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12313 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12327 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12341 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12352 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12363 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12373 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12383 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12393 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12401 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12417 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12427 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12439 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12453 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12467 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12477 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12487 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12497 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12509 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12523 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12539 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12553 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12565 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12579 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12591 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12603 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"



 




#line 12628 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12646 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12660 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12686 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12740 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12794 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12814 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12830 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12844 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12864 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12880 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12896 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 12946 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13000 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13016 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13032 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13102 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13152 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13208 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13232 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13246 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13290 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13305 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13320 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13335 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13350 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13370 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13392 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13406 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13420 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13435 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13454 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13468 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13484 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13508 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13532 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13552 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13578 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13610 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13646 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13682 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13714 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13729 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13744 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13758 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13774 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13791 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13808 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13824 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13838 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13853 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13868 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13884 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13898 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13912 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13926 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13940 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13954 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13968 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13982 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 13996 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14010 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14024 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14038 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14049 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14057 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14071 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14083 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14094 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14108 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14122 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14142 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14160 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14172 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14182 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14200 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14210 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14221 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14239 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14251 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14283 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14295 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14309 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14323 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14333 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14355 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14405 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14455 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14471 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14483 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14493 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14509 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14521 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14533 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14579 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14629 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14641 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14653 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14719 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14765 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14817 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14837 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14847 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14887 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14898 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14909 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14920 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14931 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14947 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14967 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14977 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14987 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 14998 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15011 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15021 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15033 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15053 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15073 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15089 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15111 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15139 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15171 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15203 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15231 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15242 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15253 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15263 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15275 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15288 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15301 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15313 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15323 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15334 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15345 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15357 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15367 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15377 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15387 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15397 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15407 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15417 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15427 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15437 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15447 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15457 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15467 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15478 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15492 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15558 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15604 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15656 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15676 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15686 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15726 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15772 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15822 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15834 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15846 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15866 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15885 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15903 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15917 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15953 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15975 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 15991 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16009 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16045 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16059 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16073 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16083 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16095 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16111 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16125 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"



 




#line 16148 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16164 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16180 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16196 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16210 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16226 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16240 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16256 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16270 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16286 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16300 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16316 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16374 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16436 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16452 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16472 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16488 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16508 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16522 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16580 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16642 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16658 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16678 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16694 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16714 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16770 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16830 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16844 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16858 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16872 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16882 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16892 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16902 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16916 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16930 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16944 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16959 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16977 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 16991 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17005 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17059 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17113 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17163 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17173 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17187 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17221 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17235 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17251 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17269 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17285 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17299 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17313 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17333 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17359 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17375 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17397 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17419 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17433 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17447 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17461 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17475 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17489 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17503 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17517 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17535 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17561 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17577 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17603 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17619 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17641 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17663 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17679 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17689 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17703 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17719 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17733 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17747 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17761 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17775 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17789 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17803 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17819 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17833 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17849 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17863 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17877 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17891 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17905 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17919 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17933 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17947 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17961 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17975 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 17989 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18003 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18017 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18031 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18045 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18062 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18079 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18104 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18129 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18143 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18159 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18195 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18235 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18275 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18315 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18329 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18343 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18353 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18363 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18377 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18387 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18397 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18407 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18421 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18431 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18441 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18455 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18465 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18475 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18489 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18499 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18509 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18523 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18533 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18543 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18557 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18567 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18577 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18591 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18601 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18615 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18629 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18643 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18657 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"

#line 18677 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/msmhwioreg.h"


#line 42 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/src/boot_msm.h"
#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/msmhwio.h"




 









 









  



 







 
#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/HALhwio.h"




 







 









  



 




 
#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/HALcomdef.h"





























 




 
#line 117 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/HALcomdef.h"



#line 34 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/HALhwio.h"





 




  







 









 








 





  













 
























 















 





























 


















 







































 


































 
#line 265 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/HALhwio.h"
 











 


 



   


 





 
#line 330 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/HALhwio.h"

#line 355 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/HALhwio.h"








 






 
#line 377 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/HALhwio.h"


#line 406 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/HALhwio.h"




 
#line 422 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/HALhwio.h"




 
#line 438 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/HALhwio.h"




 
#line 454 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/HALhwio.h"

 



#line 39 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/msmhwio.h"





 










 


 






 









#line 91 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/msmhwio.h"






 
#line 130 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/msmhwio.h"









 


























 
#line 174 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/systemdrivers/msmhwio.h"


 



#line 43 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/src/boot_msm.h"
#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/src/boot_error_handler.h"













 





















 





 
#line 202 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/src/boot_error_handler.h"






 
















 
#line 232 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/src/boot_error_handler.h"


#line 44 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/src/boot_msm.h"





 

 




 



 




 



 





 





























#line 55 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/sbl1/sbl1.s"
#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/boot_target.h"















 



































 





 




#line 1 "./custwaadanaza.h"






 

#line 1 "./targwaadanaza.h"






 

#line 136 "./targwaadanaza.h"




#line 11 "./custwaadanaza.h"











#line 64 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/boot_target.h"



 





 




#line 103 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/boot_target.h"








  


 






 






 





 









 




 







 









 






 


  




 





 





 




 




 




 





 




 




 




 




 




 




                                 


                                 
                                 


                                 
                               


                                 
                               


                                 
 


                                 
 


                             
                                 



 






 





                             


                             
#line 296 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/boot_target.h"




  
 






 



                              


                              


 



   


 


 


 




 


#line 56 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/boot/secboot3/hw/msm8909/sbl1/sbl1.s"

;============================================================================
;
;                             MODULE DEFINES
;
;============================================================================
;
Mode_SVC                EQU    0x13
Mode_ABT                EQU    0x17
Mode_UND                EQU    0x1b
Mode_USR                EQU    0x10
Mode_FIQ                EQU    0x11
Mode_IRQ                EQU    0x12
Mode_SYS                EQU    0x1F

I_Bit                   EQU    0x80
F_Bit                   EQU    0x40
A_Bit			EQU    0x100



;============================================================================
;
;                             MODULE IMPORTS
;
;============================================================================

    ; Import the external symbols that are referenced in this module.
    IMPORT |Image$$SBL1_SVC_STACK$$ZI$$Limit|
    IMPORT |Image$$SBL1_UND_STACK$$ZI$$Limit|
    IMPORT |Image$$SBL1_ABT_STACK$$ZI$$Limit|
    IMPORT boot_undefined_instruction_c_handler
    IMPORT boot_swi_c_handler
    IMPORT boot_prefetch_abort_c_handler
    IMPORT boot_data_abort_c_handler
    IMPORT boot_reserved_c_handler
    IMPORT boot_irq_c_handler
    IMPORT boot_fiq_c_handler
    IMPORT boot_nested_exception_c_handler
    IMPORT sbl1_main_ctl
    IMPORT boot_crash_dump_regs_ptr

;============================================================================
;
;                             MODULE EXPORTS
;
;============================================================================

    ; Export the external symbols that are referenced in this module.
    EXPORT sbl_loop_here
    EXPORT boot_read_l2esr
   
    ; Export the symbols __main and _main to prevent the linker from
    ; including the standard runtime library and startup routine.
    EXPORT   __main
    EXPORT   _main
    EXPORT  sbl_save_regs
    EXPORT  sbl1_external_abort_enable
	EXPORT  sbl_memcpy
;============================================================================
;
;                             MODULE DATA AREA
;
;============================================================================

;
; Area that defines the register data structure
;
    AREA    SAVE_REGS, DATA

arm_core_t RN     r7 
           MAP    0,arm_core_t

svc_r0     FIELD  4
svc_r1     FIELD  4
svc_r2     FIELD  4
svc_r3     FIELD  4
svc_r4     FIELD  4
svc_r5     FIELD  4
svc_r6     FIELD  4
svc_r7     FIELD  4
svc_r8     FIELD  4
svc_r9     FIELD  4
svc_r10    FIELD  4
svc_r11    FIELD  4
svc_r12    FIELD  4
svc_sp     FIELD  4
svc_lr     FIELD  4
svc_spsr   FIELD  4
svc_pc     FIELD  4
cpsr       FIELD  4
sys_sp     FIELD  4
sys_lr     FIELD  4
irq_sp     FIELD  4
irq_lr     FIELD  4
irq_spsr   FIELD  4
abt_sp     FIELD  4
abt_lr     FIELD  4
abt_spsr   FIELD  4
udf_sp     FIELD  4
udf_lr     FIELD  4
udf_spsr   FIELD  4
fiq_r8     FIELD  4
fiq_r9     FIELD  4
fiq_r10    FIELD  4
fiq_r11    FIELD  4
fiq_r12    FIELD  4
fiq_sp     FIELD  4
fiq_lr     FIELD  4
fiq_spsr   FIELD  4

    PRESERVE8
    AREA    SBL1_VECTORS, CODE, READONLY, ALIGN=4
    CODE32
unused_reset_vector
    B     0x00000000
undefined_instruction_vector
    B     sbl1_undefined_instruction_nested_handler
swi_vector
    B     boot_swi_c_handler
prefetch_abort_vector
    B     sbl1_prefetch_abort_nested_handler
data_abort_vector
    B     sbl1_data_abort_nested_handler
reserved_vector
    B     boot_reserved_c_handler
irq_vector
    B     boot_irq_c_handler
fiq_vector
    B     boot_fiq_c_handler


;============================================================================
; Qualcomm SECONDARY BOOT LOADER 1 ENTRY POINT
;============================================================================

    AREA  SBL1_ENTRY, CODE, READONLY, ALIGN=4
    CODE32
    ENTRY
    
__main
_main

;============================================================================
;   We contiue to disable interrupt and watch dog until we jump to apps
;============================================================================
sbl1_entry

    ;Change to Supervisor Mode
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit

    ; Save the passing parameter from PBL
    mov     r7, r0

    ; Set VBAR (Vector Base Address Register) to SBL vector table
    ldr     r0, =0x08006000
    MCR     p15, 0, r0, c12, c0, 0
	
    ; Setup the supervisor mode stack
    ldr     r0, =|Image$$SBL1_SVC_STACK$$ZI$$Limit|
    mov     r13, r0

    ; Switch to IRQ
    msr     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
    mov     r13, r0

    ; Switch to FIQ
    msr     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
    mov     r13, r0	
	
    ; Switch to undefined mode and setup the undefined mode stack
    msr     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit
    ldr     r0, =|Image$$SBL1_UND_STACK$$ZI$$Limit|
    mov     r13, r0

    ; Switch to abort mode and setup the abort mode stack
    msr     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit
    ldr     r0, =|Image$$SBL1_ABT_STACK$$ZI$$Limit|
    mov     r13, r0

    ; Return to supervisor mode
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit

    ; Restore the passing parameter
    mov     r0, r7
    
    ; ------------------------------------------------------------------
    ; Call functions external to perform SBL1 function.
    ; It should never return.
    ; ------------------------------------------------------------------
    ldr    r5, =sbl1_main_ctl
    blx    r5

    ; For safety
    bl loophere  ; never returns, keep lr in r14 for debug


;============================================================================
; sbl_save_regs
;
; PROTOTYPE
;   void sbl_save_regs();
;
; ARGS
;   None
;
; DESCRIPTION
;   Configure VBAR, vector table base register.
;   
;============================================================================    
sbl_save_regs	

    ; Save CPSR
    stmfd   sp!, {r6}
    mrs     r6, CPSR

    stmfd   sp!, {r7}

    ; Switch to SVC mode
    msr     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit

    ; Capture User Mode r0-r14 (no SPSR)
    ; Registers are stored in svc structure for backwards compatibility
    ldr     arm_core_t,=boot_crash_dump_regs_ptr
    ldr     arm_core_t, [r7]
    str     r0,  svc_r0
    str     r1,  svc_r1
    str     r2,  svc_r2
    str     r3,  svc_r3
    str     r4,  svc_r4
    str     r5,  svc_r5
    ; Store r6 later after restoring it from the stack
    ; Store r7 later after restoring it from the stack
    str     r8,  svc_r8
    str     r9,  svc_r9
    str     r10, svc_r10
    str     r11, svc_r11    
    str     r12, svc_r12
    str     r14, svc_lr

    ; Store SP value
    str     sp, svc_sp

    ; Store SPSR
    mrs     r0, SPSR
    str     r0, svc_spsr

    ; Store the PC for restoration later
    ldr     r0, =sbl_save_regs
    str     r0, svc_pc

    ; Save SYS mode registers
    msr     CPSR_c, #Mode_SYS:OR:I_Bit:OR:F_Bit
    str     r13, sys_sp
    str     r14, sys_lr

    ; Save IRQ mode registers
    msr     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
    str     r13, irq_sp
    str     r14, irq_lr
    mrs     r0, SPSR
    str     r0, irq_spsr    

    ; Save ABT mode registers
    msr     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit
    str     r13, abt_sp
    str     r14, abt_lr
    mrs     r0, SPSR
    str     r0, abt_spsr  

    ; Save UND mode registers
    msr     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit
    str     r13, udf_sp
    str     r14, udf_lr
    mrs     r0, SPSR
    str     r0, udf_spsr  

    ; Save FIQ mode registers
    msr     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
    str     r8,  fiq_r8
    str     r9,  fiq_r9
    str     r10, fiq_r10
    str     r11, fiq_r11    
    str     r12, fiq_r12    
    str     r13, fiq_sp
    str     r14, fiq_lr
    mrs     r0, SPSR
    str     r0, fiq_spsr  

    ; Switch back to original mode using r6 which holds the cpsr
    msr     CPSR_c, r6

    ; Store CPSR
    str     r6, cpsr
    
    ; Restore r7 value 
    ldmfd   sp!, {r2}
    str     r2, svc_r7

    ; Restore r6 value
    ldmfd   sp!, {r1}
    str     r1, svc_r6
    mov     r6, r1
    mov     r7, r2	

    ; Finished so return    
    bx lr
	
;======================================================================
;
; void sbl_memcpy(uint32* dst, uint32* src, int32 size)
;
; We perform our copy by using assembly.
; Parameters:
;   r0 contains the destination address
;   r1 is the source address
;   r2 is the size to be copied.
; Notes:
;   - Does not handle source/destination overlapping 
;======================================================================
sbl_memcpy
   cmp      r2,   #0x80             ; check bigger than 128 bytes or not.
   bge      loop_for_128byte_copy
check_32byte_left_over
   cmp      r2,   #0x20             ; check bigger than 32 bytes or not.
   bge      loop_for_32byte_copy
check_4byte_left_over
   cmp      r2,   #0x4
   bge      loop_for_4byte_copy
check_byte_left_over
   cmp      r2,   #0x0
   bgt      loop_for_byte_copy
memcpy_end
   bx       lr
loop_for_128byte_copy
   vldm     r1!, {Q0-Q7}            ; copy 128 bytes at a time
   vstm     r0!, {Q0-Q7}
   sub      r2, r2, #0x80           ; decrement size by 128
   cmp      r2, #0x80
   bge      loop_for_128byte_copy
   blt      check_32byte_left_over
loop_for_32byte_copy
   vldm     r1!, {Q0-Q1}            ; copy 32 bytes at a time
   vstm     r0!, {Q0-Q1}
   sub      r2, r2, #0x20           ; decrement size by 32
   cmp      r2, #0x20
   bge      loop_for_32byte_copy
   blt      check_4byte_left_over
loop_for_4byte_copy
   ldmia    r1!,  {r3}              ; load 4 bytes each time
   stmia    r0!,  {r3}
   sub      r2,r2,#0x4;
   cmp      r2,#0x4
   bge      loop_for_4byte_copy
   blt      check_byte_left_over
loop_for_byte_copy
   ldrb     r3,   [r1], #0x1
   strb     r3,   [r0], #0x1
   sub      r2,r2,#0x1
   cmp      r2, #0x0
   bgt      loop_for_byte_copy
   ble      memcpy_end              ; end the byte copy.
   
;======================================================================
; Called by sbl1_error_handler only. We clean up the registers and loop
; here until JTAG is connected.
;======================================================================
sbl_loop_here
    mov r0,#0
    mov r1,#0
    mov r2,#0
    mov r3,#0
    mov r4,#0
    mov r5,#0
    mov r6,#0
    mov r7,#0
    mov r8,#0
    mov r9,#0
    mov r10,#0
    mov r11,#0
    mov r12,#0
loophere
    b loophere


;======================================================================
; SBL1 exception handlers that can have nested calls to them.  These
; handlers check for nesting and if it is the first exception they
; call a "C" exception handler that calls the SBL1 error handler.
; If it is a nested exception, the "C" exception handler is not
; re-entered and the JTAG interface is enabled immediately. Nesting
; is only a concern for undefined instruction and abort exceptions.
; Note, a separate exception handler is used for each exception to
; provide additional debug information (see sbl1_error_handler.c for
; more information).
;======================================================================

	
sbl1_undefined_instruction_nested_handler
    ldr r5,=boot_undefined_instruction_c_handler
    b   check_for_nesting

sbl1_prefetch_abort_nested_handler
    ldr r5,=boot_prefetch_abort_c_handler
    b   check_for_nesting

sbl1_data_abort_nested_handler
    ldr r5,=boot_data_abort_c_handler
    b   check_for_nesting

;======================================================================
; Checks for nested exceptions and then calls the "C" exception
; handler pointed to by R5 if this is the first time this exception
; has occurred, otherwise it calls the "C" nested exception handler
; that just enables JTAG debug access.  The mode stack pointer is used
; to determine if a nested exception or a second abort exception has
; occurred.  This is accomplished by comparing the mode stack pointer
; to the top of the stack that was initially assigned to the stack.
; If they are equal, it is a first time exception.
;======================================================================
check_for_nesting

    ; Initial stack base depends on the current processor mode
    ; Mode will either be ABT or UND.  Load proper stack base.
    mrs r7, cpsr
    and r7, r7, #Mode_SYS ; Use Mode_SYS for mode bitmask as all bits are high
    cmp r7, #Mode_ABT
    ldreq r6,=|Image$$SBL1_ABT_STACK$$ZI$$Limit|
    cmp r7, #Mode_UND
    ldreq r6,=|Image$$SBL1_UND_STACK$$ZI$$Limit|

    mov r7, r13                            ; Save current stack ptr
    cmp r6, r7                             ; Compare initial and actual
    blxeq r5                               ; First time exception
    ldr r5,=boot_nested_exception_c_handler; This is a nested exception
    blx r5

boot_read_l2esr
    mov r1, #0x204    ;Indirect address of L2ESR
    isb
    mcr p15,3,r1,c15,c0,6 ;Write the L2CPUCPSELR with the indirect address of the L2ESR
    isb
    mrc p15,3,r0,c15,c0,7 ;store L2ESR to r0
    isb
    bx lr
    
; void sbl1_external_abort_enable(uint32 flags)
sbl1_external_abort_enable FUNCTION
    and     r0, r0, #F_Bit:OR:I_Bit:OR:A_Bit 	; Only care about A/I/F bits.
    mrs     r1, cpsr                            ; Read the status register.
    bic     r1, r1, r0                          ; Clear requested A/I/F bits
    msr     cpsr_cx, r1                         ; Write control & extension field
    bx      lr
    ENDFUNC	
    END
