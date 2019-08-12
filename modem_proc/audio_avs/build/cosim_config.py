#===============================================================================
#
# AVS Cosim configuration for simulation
#
# GENERAL DESCRIPTION
#    AVS Cosim configuration for simulation
#     
#
# Copyright (c) 2009-2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/build/cosim_config.py#1 $
#  $DateTime: 2016/12/13 07:58:24 $
#  $Author: mplcsds1 $
#  $Change: 11985135 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 09/21/12   kk      AVS Cosim configuration
#
#===============================================================================

import re
import string
import os
import sys
def generate_cosim_config(env):
    ADSP_AFE_COSIM = "adsptest/Audio/drivers/afe/AFECosim8660/scfAudioIfCosim"
    print "----"+env.get('PLATFORM')
    if env.get('PLATFORM') in ["Windows_NT","windows","win32","cygwin"]:
        if env.get('CHECKSIMBOOT') == True:
        
            PLATFORM_DIR="cygwin"    
            COSIM_EXT="w32"
            LIB_EXT="dll"
        else:
                # <-- HACK! --> Force Linux path always. PW cannot build in Linux. So have to use
        #windows builds to test in Linux. Clean-up when Linux builds work in PW.
            PLATFORM_DIR="lnx64"    
            COSIM_EXT="lnx64"
            LIB_EXT="so"
    elif env.get('PLATFORM') == 'linux':
        PLATFORM_DIR="lnx64"    
        COSIM_EXT="lnx64"
        LIB_EXT="so"
    else:
        print "Un supported platform"
        sys.exit(0)
    ADSP_AFE_COSIM_WITH_EXT=ADSP_AFE_COSIM+"_"+COSIM_EXT+"."+LIB_EXT
    BUILD_TARGET = env.get('BUILD_ASIC')
    if BUILD_TARGET in ["8974","9x25","8x10","8x26","9x35","8926"]:
        COSIM_TARGET ="8974"

    if BUILD_TARGET in ["8092","8084"]:
        COSIM_TARGET ="8084"
    
    if BUILD_TARGET in ["8916","8936","8909","9607"]:
       TIMER_BASE = "0x04100000"
       L2VIC_BASE = "0x04110000"
       COSIM_TARGET ="8916"
    else:
       TIMER_BASE = "0xfe280000"
       L2VIC_BASE = "0xfe290000"

    DMAIOFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/dma_io.cfg"
    AVTIOFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/avtimerCosim.cfg"
    BTFMIOFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/BTFM.cfg"
    DMCOSIMFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/scfdm_cosim_"+COSIM_EXT+"."+LIB_EXT
    DMACFGFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/scfAudioIfCosim_"+COSIM_EXT+"."+LIB_EXT
    MIDICOSIMFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/scfmidiCosim_"+COSIM_EXT+"."+LIB_EXT
    AVTCOSIMFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/scfavtimerCosim_"+COSIM_EXT+"."+LIB_EXT
    RSCOSIMFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/resamplerCosim_"+COSIM_EXT+"."+LIB_EXT
    VFRCOSIMFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/vfrGenerator_"+COSIM_EXT+"."+LIB_EXT
    VIDCOSIMFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/vid_hw_workaround_cosim_"+COSIM_EXT+"."+LIB_EXT
    BTFMCOSIMFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/scfbtfmCosim_"+COSIM_EXT+"."+LIB_EXT
    DALCHIPINFOCOSIMFILE = "afe/drivers/cosims/common/dalchip_Cosim_"+COSIM_EXT+"."+LIB_EXT

    if BUILD_TARGET == "8092":
        HDMICAPTURECOSIMFILE = "afe/drivers/cosims/8092/HdmiCaptureCosim_"+COSIM_EXT+"."+LIB_EXT
        ATIMEIOFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/atimeCosim.cfg"
        ATIME2IOFILE = "afe/drivers/cosims/"+BUILD_TARGET+"/atime2Cosim.cfg"
        HDMICAPTUREIOFILE = "afe/drivers/cosims/8092/hdmi_capture_properties.cfg"
                
    if BUILD_TARGET == "8084":
        ATIMEIOFILE = "afe/drivers/cosims/"+COSIM_TARGET+"/atimeCosim.cfg"
        
    DALCHIPINFOPARAMS = " "+env.get('BUILD_ASIC')
    
    if COSIM_TARGET == "8974":
        BTFMCOSIMPARAMS = " -ctl_reg 0xA206000\
                            -l2vicbaseaddr 0xfe290000\
                            -BT_SPKR_IntNum 47\
                            -BT_MIC_IntNum 44\
                            -MailBox_IntNum 40\
                            -FM_IntNum 56\
                            -Cfg_FileName "+BTFMIOFILE+"\
                            -lpass_cc_baseaddr 0xfe000000"
        DMCOSIMPARAMS =  " 0xFE04A000--0xFE04A014-10\
                     -l2vicbaseaddr 0xfe290000\
                     -l2vicintrnum 23"
        DMACFGPARAMS = " -aifstartaddr 0x7708000\
                        -aifendaddr 0x7717FF0\
                        -aifresetaddr 0x7717FF4\
                        -buslatency 30\
                        -l2vicbaseaddr 0xfe290000\
                        -l2vicintrnum 46\
                        -aifcfg "+DMAIOFILE+"\
                        -timer_STC_Addr 0x770600C"
        MIDICFGPARAMS = " -midistartaddr 0xFE070000\
                    -midiendaddr 0xFE07FFFF\
                    -buslatency 30\
                    -l2vicbaseaddr 0xfe290000\
                    -l2vicintrnum 22"
        AVTCOSIMPARAMS = " -l2vicbaseaddr 0xfe290000\
                        -l2vicintrnum 45\
                        -avtstartaddr 0x7706000\
                        -avtendaddr 0x7706FF0\
                        -buslatency 30\
                        -avtimercfg "+AVTIOFILE
        RSCFGPARAMS = " -rsstartaddr 0xFE058000\
                  -rsendaddr 0xFE05BFFF\
                  -buslatency 30\
                  -l2vicbaseaddr 0xfe290000\
                  -l2vicintrnum 53"
        
                                
        if BUILD_TARGET == "8x26":
                VFR1CFGPARAMS = " -trigger_lpaif TRUE\
                      -init_delay_us 0\
                   -vfr_period_us 20000\
                  -lpaif_baseaddr 0xFE0C0000\
                  -l2vicbaseaddr 0xfe290000\
                  -l2vicintrnum 36\
                  -lpass_cc_baseaddr 0x0"
        else:
                VFR1CFGPARAMS = " -trigger_lpaif TRUE\
                                -init_delay_us 0\
                                -vfr_period_us 20000\
                                -lpaif_irq_force_addr 0x0770F010\
                                -lpaif_irq_force_mask 0x08000000\
                                -l2vicbaseaddr 0xfe290000\
                                -l2vicintrnum 59\
                                -lpass_cc_baseaddr 0x0\
                                -vfrID 0"

                VFR2CFGPARAMS = " -trigger_lpaif FALSE\
                                -init_delay_us 10000\
                                -vfr_period_us 20000\
                                -lpaif_irq_force_addr 0x0770F010\
                                -lpaif_irq_force_mask 0x08000000\
                                -l2vicbaseaddr 0xfe290000\
                                -l2vicintrnum 60\
                                -lpass_cc_baseaddr 0x0\
                                -vfrID 7"
        
    if COSIM_TARGET == "8084":
        BTFMCOSIMPARAMS = " -ctl_reg 0xFB206000\
                        -l2vicbaseaddr 0xfe290000\
                        -BT_SPKR_IntNum 10\
                        -BT_MIC_IntNum 17\
                        -MailBox_IntNum 15\
                        -FM_IntNum 16\
                        -Cfg_FileName "+BTFMIOFILE
        DMCOSIMPARAMS =  " 0xFE0B2000--0xFE0B2014-10\
                         -l2vicbaseaddr 0xfe290000\
                         -l2vicintrnum 23"
        DMACFGPARAMS = " -aifstartaddr 0xFE140000\
                           -aifendaddr 0xFE15FFF0\
                           -aifresetaddr 0xFE15FFF4\
                           -buslatency 30\
                           -l2vicbaseaddr 0xfe290000\
                           -l2vicintrnum 19\
                           -aifcfg "+DMAIOFILE
        MIDICFGPARAMS = " -midistartaddr 0xFE070000\
                        -midiendaddr 0xFE07FFFF\
                        -buslatency 30\
                        -l2vicbaseaddr 0xfe290000\
                        -l2vicintrnum 22"
        AVTCOSIMPARAMS = " -l2vicbaseaddr 0xfe290000\
                         -l2vicintrnum 50\
                         -avtstartaddr 0xFE0C9000\
                         -avtendaddr 0xFE0C93FF\
                         -buslatency 30\
                         -avtimercfg "+AVTIOFILE
        
        if BUILD_TARGET == "8092":
                ATCOSIMPARAMS_1 = " -l2vicbaseaddr 0xfe290000\
                                 -l2vicintrnum 50\
                                 -avtstartaddr 0xFE0CB000\
                                 -avtendaddr 0xFE0CB3FF\
                                 -buslatency 30\
                                 -avtimercfg "+ATIMEIOFILE
                ATCOSIMPARAMS_2 = " -l2vicbaseaddr 0xfe290000\
                                 -l2vicintrnum 50\
                                 -avtstartaddr 0xFE0CF000\
                                 -avtendaddr 0xFE0CF3FF\
                                 -buslatency 30\
                                 -avtimercfg "+ATIME2IOFILE
                HDMICOSIMPARAMS = " -hdmicapturestartaddr 0xFE134000\
                                 -hdmicaptureendaddr 0xFE1340D0\
                                 -buslatency 30\
                                 -hdmicfg "+HDMICAPTUREIOFILE+"\
                                 -hdmiresetaddr 0xFE1340D4\
                                 -l2vicbaseaddr 0xfe290000\
                                 -l2vicintrnum 121\
                                 -timer_STC_reg_addr 0xFE0C900C\
                                 -appl2vicintrnum 61\
                                 -trace_level 0\
                                 -trace_reg_writes 0\
                                 -trace_reg_reads 0"
                                                         
        elif BUILD_TARGET == "8084":
                ATCOSIMPARAMS_1 = " -l2vicbaseaddr 0xfe290000\
                 -l2vicintrnum 50\
                 -avtstartaddr 0xFE0C9400\
                 -avtendaddr 0xFE0C998F\
                 -buslatency 30\
                 -avtimercfg "+ATIMEIOFILE
        
        RSCFGPARAMS = " -rsstartaddr 0xFE0D0000\
                          -rsendaddr 0xFE0D0FFF\
                          -buslatency 30\
                          -l2vicbaseaddr 0xfe290000\
                          -l2vicintrnum 53"
                        
        VFR1CFGPARAMS = " -trigger_lpaif TRUE\
          -init_delay_us 0\
          -vfr_period_us 20000\
          -lpaif_baseaddr 0xFE03B000\
          -l2vicbaseaddr 0xfe290000\
          -l2vicintrnum 51\
          -lpass_cc_baseaddr 0x0"

        VFR2CFGPARAMS = " -trigger_lpaif FALSE\
                          -init_delay_us 10000\
                          -vfr_period_us 20000\
                          -lpaif_baseaddr 0x0\
                          -l2vicbaseaddr 0xfe290000\
                          -l2vicintrnum 40\
                          -lpass_cc_baseaddr 0x0"

    if BUILD_TARGET in ["8916","8936","8909"]:
        BTFMCOSIMPARAMS = " -ctl_reg 0xA206000\
                            -l2vicbaseaddr 0x04110000\
                            -BT_SPKR_IntNum 47\
                            -BT_MIC_IntNum 44\
                            -MailBox_IntNum 40\
                            -FM_IntNum 56\
                            -Cfg_FileName "+BTFMIOFILE+"\
                            -lpass_cc_baseaddr 0xe4000000"
        DMCOSIMPARAMS =  " 0xFE04A000--0xFE04A014-10\
                     -l2vicbaseaddr 0x04110000\
                     -l2vicintrnum 23"
        DMACFGPARAMS = " -aifstartaddr 0x7708000\
                        -aifendaddr 0x7717FF0\
                        -aifresetaddr 0x7717FF4\
                        -buslatency 30\
                        -l2vicbaseaddr 0x04110000\
                        -l2vicintrnum 46\
                        -aifcfg "+DMAIOFILE+"\
                        -timer_STC_Addr 0x770600C"
        MIDICFGPARAMS = " -midistartaddr 0xFE070000\
                    -midiendaddr 0xFE07FFFF\
                    -buslatency 30\
                    -l2vicbaseaddr 0x04110000\
                    -l2vicintrnum 22"
        AVTCOSIMPARAMS = " -l2vicbaseaddr 0x04110000\
                        -l2vicintrnum 45\
                        -avtstartaddr 0x7706000\
                        -avtendaddr 0x7706FF0\
                        -buslatency 30\
                        -avtimercfg "+AVTIOFILE
        RSCFGPARAMS = " -rsstartaddr 0xFE058000\
                  -rsendaddr 0xFE05BFFF\
                  -buslatency 30\
                  -l2vicbaseaddr 0x04110000\
                  -l2vicintrnum 53"
        
                                
        if BUILD_TARGET == "8x26":
                VFR1CFGPARAMS = " -trigger_lpaif TRUE\
                      -init_delay_us 0\
                   -vfr_period_us 20000\
                  -lpaif_baseaddr 0xFE0C0000\
                  -l2vicbaseaddr 0x04110000\
                  -l2vicintrnum 36\
                  -lpass_cc_baseaddr 0x0"
        else:
                VFR1CFGPARAMS = " -trigger_lpaif TRUE\
                                -init_delay_us 0\
                                -vfr_period_us 20000\
                                -lpaif_irq_force_addr 0x0770F010\
                                -lpaif_irq_force_mask 0x08000000\
                                -l2vicbaseaddr 0x04110000\
                                -l2vicintrnum 59\
                                -lpass_cc_baseaddr 0x0\
                                -vfrID 0"

                VFR2CFGPARAMS = " -trigger_lpaif FALSE\
                                -init_delay_us 10000\
                                -vfr_period_us 20000\
                                -lpaif_irq_force_addr 0x0770F010\
                                -lpaif_irq_force_mask 0x08000000\
                                -l2vicbaseaddr 0x04110000\
                                -l2vicintrnum 60\
                                -lpass_cc_baseaddr 0x0\
                                -vfrID 7"

    
    if env.get('Q6VERSION')=="v5":
        if env.get('PLATFORM') in ["Windows_NT","windows","win32","cygwin"]:
            if env.get('CHECKSIMBOOT') == True:
                
                TIMERCFGFILE = "C:/Qualcomm/HEXAGON_Tools/"+env.get('HEXAGON_RTOS_RELEASE')+"/qc/lib/iss/qtimer."+LIB_EXT
                L2VICCFGFILE = "C:/Qualcomm/HEXAGON_Tools/"+env.get('HEXAGON_RTOS_RELEASE')+"/qc/lib/iss/l2vic."+LIB_EXT
                TIMERCFGPARAMS = " --csr_base="+TIMER_BASE+" --irq_p=2,3 --freq=19200000 --cnttid=17"
                L2VICCFGPARAMS = " 32 "+L2VIC_BASE
            else:
                # <-- HACK! --> Force Linux path always. PW cannot build in Linux. So have to use
                #windows builds to test in Linux. Clean-up when Linux builds work in PW.
                TIMERCFGFILE = "/pkg/qct/software/hexagon/releases/tools/"+env.get('HEXAGON_RTOS_RELEASE')+"/qc/lib/iss/qtimer."+LIB_EXT
                L2VICCFGFILE = "/pkg/qct/software/hexagon/releases/tools/"+env.get('HEXAGON_RTOS_RELEASE')+"/qc/lib/iss/l2vic."+LIB_EXT
                TIMERCFGPARAMS = " --csr_base="+TIMER_BASE+" --irq_p=2,3 --freq=19200000 --cnttid=17"
                L2VICCFGPARAMS = " 32 "+L2VIC_BASE
        elif env.get('PLATFORM') == 'linux':	
            TIMERCFGFILE = "/pkg/qct/software/hexagon/releases/tools/"+env.get('HEXAGON_RTOS_RELEASE')+"/qc/lib/iss/qtimer."+LIB_EXT
            L2VICCFGFILE = "/pkg/qct/software/hexagon/releases/tools/"+env.get('HEXAGON_RTOS_RELEASE')+"/qc/lib/iss/l2vic."+LIB_EXT
            TIMERCFGPARAMS = " --csr_base="+TIMER_BASE+" --irq_p=2,3 --freq=19200000 --cnttid=17"
            L2VICCFGPARAMS = " 32 "+L2VIC_BASE
        else:
            print "Unsupported flavor(supports only on windows and Linux)"
            sys.exit(0)
    else:
        TIMERCFGFILE = "/pkg/qct/software/hexagon/releases/tools/"+env.get('HEXAGON_RTOS_RELEASE')+"/qc/lib/iss/qtimer."+LIB_EXT
        L2VICCFGFILE = "/pkg/qct/software/hexagon/releases/tools/"+env.get('HEXAGON_RTOS_RELEASE')+"/qc/lib/iss/l2vic."+LIB_EXT
        TIMERCFGPARAMS = " --csr_base="+TIMER_BASE+" --irq_p=2,3 --freq=19200000 --cnttid=17"
        L2VICCFGPARAMS = " 32 "+L2VIC_BASE              

            
    Q6SS_OBJ_FILE_PATH = "../"
    Q6SS_OBJ_FILE_TEMP = Q6SS_OBJ_FILE_PATH+"q6ss1.cfg"
    Q6SS_OBJ_FILE = Q6SS_OBJ_FILE_PATH+"q6ss.cfg"
    ADSP_TEST_COSIMOPTS_FILE = "../adsp_test_cosimopts.cfg"
    ADSP_TEST_INTERNAL_BT_FM_COSIMOPTS_FILE = "../adsp_test_internal_bt_fm_cosimopts.cfg"
    DMAIOFILE_PATH="../"+DMAIOFILE
    BTFMIOFILE_PATH="../"+BTFMIOFILE

    if env.get('PLATFORM') in ["Windows_NT","windows","win32","cygwin"]:
        # <-- HACK! --> Force Linux path always. PW cannot build in Linux. So have to use
        #windows builds to test in Linux. Clean-up when Linux builds work in PW.
        if env.get('CHECKSIMBOOT') == True:    
            TIMERCFGFILE = TIMERCFGFILE.replace("/","\\")
            DMACFGFILE = DMACFGFILE.replace("/","\\")
            MIDICOSIMFILE = MIDICOSIMFILE.replace("/","\\")
            DMCOSIMFILE = DMCOSIMFILE.replace("/","\\")
            L2VICCFGFILE = L2VICCFGFILE.replace("/","\\")
            AVTCOSIMFILE = AVTCOSIMFILE.replace("/","\\")
            VIDCOSIMFILE = VIDCOSIMFILE.replace("/","\\")
            RSCOSIMFILE = RSCOSIMFILE.replace("/","\\")
            VFRCOSIMFILE = VFRCOSIMFILE.replace("/","\\")
            BTFMCOSIMFILE = BTFMCOSIMFILE.replace("/","\\")
            Q6SS_OBJ_FILE = Q6SS_OBJ_FILE.replace("/","\\")
            Q6SS_OBJ_FILE_PATH = Q6SS_OBJ_FILE_PATH.replace("/","\\")
            ADSP_TEST_COSIMOPTS_FILE = ADSP_TEST_COSIMOPTS_FILE.replace("/","\\")
            ADSP_TEST_INTERNAL_BT_FM_COSIMOPTS_FILE = ADSP_TEST_INTERNAL_BT_FM_COSIMOPTS_FILE.replace("/","\\")
            DMAIOFILE_PATH = DMAIOFILE_PATH.replace("/","\\")
            BTFMIOFILE_PATH = BTFMIOFILE_PATH.replace("/","\\")
         

                
    
    # generate q6ss.cfg
    if env.get('PLATFORM') in ["Windows_NT","windows","win32","cygwin"] :
        if env.get('CHECKSIMBOOT') == True:
            fo_q6ss = open(Q6SS_OBJ_FILE,'wb')
            fo_q6ss.write(TIMERCFGFILE+TIMERCFGPARAMS+"\n")
            fo_q6ss.write(L2VICCFGFILE+L2VICCFGPARAMS+"\n")
            fo_q6ss.close()
        else: 
            fo_q6ss = open(Q6SS_OBJ_FILE,'wb')
            fo_q6ss.write(TIMERCFGFILE+TIMERCFGPARAMS+"\n")
            fo_q6ss.write(L2VICCFGFILE+L2VICCFGPARAMS+"\n")
            fo_q6ss.write(VIDCOSIMFILE+"\n")
            fo_q6ss.write(MIDICOSIMFILE+MIDICFGPARAMS+"\n")
            fo_q6ss.write(DMACFGFILE+DMACFGPARAMS+"\n")
            fo_q6ss.write(DMCOSIMFILE+DMCOSIMPARAMS+"\n")
            fo_q6ss.write(AVTCOSIMFILE+AVTCOSIMPARAMS+"\n")
            fo_q6ss.write(RSCOSIMFILE+RSCFGPARAMS+"\n")
            fo_q6ss.write(VFRCOSIMFILE+VFR1CFGPARAMS+"\n")
            fo_q6ss.write(VFRCOSIMFILE+VFR2CFGPARAMS+"\n")
            fo_q6ss.write(BTFMCOSIMFILE+BTFMCOSIMPARAMS+"\n")
            fo_q6ss.write(DALCHIPINFOCOSIMFILE+DALCHIPINFOPARAMS+"\n")
            fo_q6ss.close()
    elif env.get('PLATFORM') == 'linux':
        fo_q6ss = open(Q6SS_OBJ_FILE,'wb')
        fo_q6ss.write(TIMERCFGFILE+TIMERCFGPARAMS+"\n")
        fo_q6ss.write(L2VICCFGFILE+L2VICCFGPARAMS+"\n")
        fo_q6ss.write(VIDCOSIMFILE+"\n")
        fo_q6ss.write(MIDICOSIMFILE+MIDICFGPARAMS+"\n")
        fo_q6ss.write(DMACFGFILE+DMACFGPARAMS+"\n")
        fo_q6ss.write(DMCOSIMFILE+DMCOSIMPARAMS+"\n")
        fo_q6ss.write(AVTCOSIMFILE+AVTCOSIMPARAMS+"\n")
        if BUILD_TARGET == "8092":
            fo_q6ss.write(AVTCOSIMFILE+ATCOSIMPARAMS_1+"\n")
            fo_q6ss.write(AVTCOSIMFILE+ATCOSIMPARAMS_2+"\n")
            fo_q6ss.write(HDMICAPTURECOSIMFILE+HDMICOSIMPARAMS+"\n")
        elif BUILD_TARGET == "8084":
            fo_q6ss.write(AVTCOSIMFILE+ATCOSIMPARAMS_1+"\n")
        fo_q6ss.write(RSCOSIMFILE+RSCFGPARAMS+"\n")
        fo_q6ss.write(VFRCOSIMFILE+VFR1CFGPARAMS+"\n")
        fo_q6ss.write(VFRCOSIMFILE+VFR2CFGPARAMS+"\n")
        fo_q6ss.write(BTFMCOSIMFILE+BTFMCOSIMPARAMS+"\n")
        fo_q6ss.write(DALCHIPINFOCOSIMFILE+DALCHIPINFOPARAMS+"\n")
        fo_q6ss.close()
    else:
        print "Unsupported flavor"
        sys.exit(0)
     
    

    """
        forg_q6ss = open(Q6SS_OBJ_FILE,'w')
        with open(Q6SS_OBJ_FILE_TEMP,'r') as f:
            for line in f:
                line_mod = re.sub('\r', '', line)
                forg_q6ss.write(line_mod)

        forg_q6ss.close()
        
    """


    #generate adsp_test_cosimopts.cfg
    fo_opts = open(ADSP_TEST_COSIMOPTS_FILE,'wb')
    fo_opts.write(DMACFGFILE+DMACFGPARAMS+"\n")
    fo_opts.close()

    #generate adsp_test_internal_bt_fm_cosimopts.cfg
    fo_btfm_opts = open(ADSP_TEST_INTERNAL_BT_FM_COSIMOPTS_FILE,'wb')
    fo_btfm_opts.write(BTFMCOSIMFILE+BTFMCOSIMPARAMS+"\n")
    fo_btfm_opts.close()




    #Making DMA IO File Writeable, for cosim reset option...
    os.chmod(DMAIOFILE_PATH,0777)

    #Making INTERNAL BT FM File Writeable, for cosim reset option...
    os.chmod(BTFMIOFILE_PATH,0777)

    return


