##############################################################################
# Australian Public Licence B (OZPLB)
# 
# Version 1-0
# 
# Copyright (c) 2007, Open Kernel Labs, Inc.
# 
# All rights reserved. 
# 
# Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
#               National ICT Australia
#               http://www.ertos.nicta.com.au
# 
# Permission is granted by National ICT Australia, free of charge, to
# any person obtaining a copy of this software and any associated
# documentation files (the "Software") to deal with the Software without
# restriction, including (without limitation) the rights to use, copy,
# modify, adapt, merge, publish, distribute, communicate to the public,
# sublicense, and/or sell, lend or rent out copies of the Software, and
# to permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimers.
# 
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimers in the documentation and/or other materials provided
#       with the distribution.
# 
#     * Neither the name of National ICT Australia, nor the names of its
#       contributors, may be used to endorse or promote products derived
#       from this Software without specific prior written permission.
# 
# EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
# PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
# NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
# WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
# BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
# REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
# THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
# ERRORS, WHETHER OR NOT DISCOVERABLE.
# 
# TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
# NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
# THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
# LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
# OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
# OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
# OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
# CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
# CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
# DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
# CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
# DAMAGES OR OTHER LIABILITY.
# 
# If applicable legislation implies representations, warranties, or
# conditions, or imposes obligations or liability on National ICT
# Australia or one of its contributors in respect of the Software that
# cannot be wholly or partly excluded, restricted or modified, the
# liability of National ICT Australia or the contributor is limited, to
# the full extent permitted by the applicable legislation, at its
# option, to:
# a.  in the case of goods, any one or more of the following:
# i.  the replacement of the goods or the supply of equivalent goods;
# ii.  the repair of the goods;
# iii. the payment of the cost of replacing the goods or of acquiring
#  equivalent goods;
# iv.  the payment of the cost of having the goods repaired; or
# b.  in the case of services:
# i.  the supplying of the services again; or
# ii.  the payment of the cost of having the services supplied again.
# 
# The construction, validity and performance of this licence is governed
# by the laws in force in New South Wales, Australia.

"""
Processing of the machine XML element.
"""

from ezxml import Element, long_attr, str_attr, size_attr
import re
import sys

Qdsp6_base_el = Element("Hexagon_ss_base",
                         value = (long_attr, "required"))
Tcxo_el = Element("tcxo",
                  intmask = (long_attr, "optional"))
QtimerBase_el = Element("base",
                      value = (long_attr, "optional"))                      
Intno_el = Element("intno",
                      value = (str_attr, "required"))
Priority_el = Element("priority",
                      value = (str_attr, "required"))
Bitmask_el = Element("bitmask",
                     value = (str_attr, "optional"))
IST_priority_el = Element("IST_priority",
                          value = (str_attr, "required"))
IST_tcb_partition_el = Element("IST_tcb_partition",
                          value = (str_attr, "required"))
IST_bitmask_el = Element("IST_bitmask",
                         value = (str_attr, "optional"))
"""
The following timer attributes are unused but kept for backward compatibility:
IST_bitmask_el, Priority_el, Bitmask_el
"""
Timer_el = Element("timer",
                   QtimerBase_el,
                   Intno_el,
                   IST_priority_el,
                   IST_tcb_partition_el,
                   IST_bitmask_el,
                   Priority_el,
                   Bitmask_el,
                   )

TimetestPort_el = Element("timetest_port",
                          value = (long_attr, "optional"))
Isdbmem_el = Element("isdb_imem",
                          addr = (long_attr, "required"))
Max_island_interrupts_el = Element("max_island_interrupts",
                          value = (long_attr, "optional"))
Chk_usrpd_interrupts_el = Element("interrupt_mpd_privilege_check",
                          enabled = (str_attr, "optional"))
Chk_hvx_power_for_locking_el = Element("check_hvx_power_for_locking",
                          enabled = (str_attr, "optional"))
Livelock_el = Element("livelock_exception",
                          pcycles = (long_attr, "required"))
ETM_to_q6etb_el = Element("etm_trace_to_q6etb",
                          enabled = (str_attr, "required"))
DMT_el = Element("dynamic_multi_threading",
                 enabled = (str_attr, "required"));
BQ_el = Element("bus_resources_shared",
                 enabled = (str_attr, "required"));
ETM_cfg_base_el = Element("etm_cfg_base",
                     addr = (long_attr, "required"))
ETM_atid_el = Element("etm_atid",
                     value = (long_attr, "required"))
Rgdr_el = Element("rgdr",
                  value = (str_attr, "optional"))
Rgdr2nd_el = Element("rgdr_2nd",
                  value = (str_attr, "optional"))
Rgdr3rd_el = Element("rgdr_3rd",
                  value = (str_attr, "optional"))
Cbits_el = Element("chicken",
                  value = (str_attr, "optional"))
Cbits2nd_el = Element("chicken_2nd",
                  value = (str_attr, "optional"))
Cbits3rd_el = Element("chicken_3rd",
                  value = (str_attr, "optional"))
Acc0_el = Element("acc0",
                  value = (str_attr, "optional"))
Acc1_el = Element("acc1",
                  value = (str_attr, "optional"))
Supplement_rgdr_value_el = Element("rgdr",
                  value = (str_attr, "required"),
                  revid = (str_attr, "required"))
Supplement_chicken_value_el = Element("chicken",
                  value = (str_attr, "required"),
                  revid = (str_attr, "required"))
Supplement_rgdr_el = Element("supplement_rgdr",
                             Supplement_rgdr_value_el)
Supplement_chicken_el = Element("supplement_chicken",
                             Supplement_chicken_value_el)

Chicken_el = Element("chicken_bits",
                     Rgdr_el,
                     Rgdr2nd_el,
                     Rgdr3rd_el,
                     Acc0_el,
                     Acc1_el,
                     Cbits_el,
                     Cbits2nd_el,
                     Cbits3rd_el,
                     Supplement_rgdr_el,
                     Supplement_chicken_el
                     )
MaxHthreads_el = Element("max_hthreads",
                         value = (str_attr, "required"))
L1_iprefetch_el = Element("l1_iprefetch",
                          enabled = (str_attr, "optional"),
                          lines = (str_attr, "optional"))
L1_dprefetch_el = Element("l1_dprefetch",
                          enabled = (str_attr, "optional"),
                          lines = (str_attr, "optional"))
L2_iprefetch_el = Element("l2_iprefetch",
                          enabled = (str_attr, "optional"),
                          lines = (str_attr, "optional"))
L2_dprefetch_el = Element("l2_dprefetch",
                          enabled = (str_attr, "optional"),
                          lines = (str_attr, "optional"))
L1_ipartition_el = Element("l1_ipartition",
                          main = (str_attr, "optional"))
L1_dpartition_el = Element("l1_dpartition",
                          main = (str_attr, "optional"))
L2_partition_el = Element("l2_partition",
                          main = (str_attr, "optional"))
L1_ic_kilobytes_el = Element("l1_icache_size",
                          value = (size_attr, "optional"))
L2_kilobytes_el = Element("l2_size",
                          value = (str_attr, "required"))
I_cache_usage_el = Element("icache_usage",
                          enabled = (str_attr, "optional"))
L2_wa_el = Element("l2_write_allocate",
                          enabled = (str_attr, "optional"))
L2_cache_writeback = Element("l2_writeback",
                             enabled = (str_attr, "optional"))
Cache_el = Element("cache",
                    L1_iprefetch_el,
                    L1_dprefetch_el,
                    L2_iprefetch_el,
                    L2_dprefetch_el,
                    L1_ipartition_el,
                    L1_dpartition_el,
                    L2_partition_el,
                    L1_ic_kilobytes_el,
                    L2_kilobytes_el,
                    I_cache_usage_el,
                    L2_wa_el,
                    L2_cache_writeback)
Cache_policy_el = Element("cache_policy",
                       name = (str_attr, "required"),
                       value =(str_attr, "required"))

from interrupt_xml import Interrupt_el               
Machine_el = Element("machine",
                     Interrupt_el,
                     Qdsp6_base_el,
                     Tcxo_el,
                     Timer_el,
                     TimetestPort_el,
                     Isdbmem_el,
                     Max_island_interrupts_el,
                     Chk_usrpd_interrupts_el,
                     Chk_hvx_power_for_locking_el,
                     Livelock_el,
                     ETM_to_q6etb_el,
                     ETM_cfg_base_el,
                     ETM_atid_el,
                     DMT_el,
                     BQ_el,
                     Chicken_el,
                     Cache_el,
                     Cache_policy_el,
                     file = (str_attr, "optional"))

   
def collect_chicken_bits_element(cfg, chicken_el):
    try:
       cfg["QURTKCHICKENBITSRGDR"] = chicken_el.find_child("rgdr").value
    except AttributeError:
       cfg["QURTKCHICKENBITSRGDR"] = "0"
    
    #QURTK_chicken_bits_rgdr_2nd
    try:
       cfg["QURTKCHICKENBITS2NDRGDR"] = chicken_el.find_child("rgdr_2nd").value
    except AttributeError:
       cfg["QURTKCHICKENBITS2NDRGDR"] = "0"

    #QURTK_chicken_bits_rgdr_3rd
    try:
       cfg["QURTKCHICKENBITS3RDRGDR"] = chicken_el.find_child("rgdr_3rd").value
    except AttributeError:
       cfg["QURTKCHICKENBITS3RDRGDR"] = "0"
    
    #QURTK_chicken_bits_chicken
    try:
       cfg["QURTKCHICKENBITSCHICKEN"] = chicken_el.find_child("chicken").value
    except AttributeError:
       cfg["QURTKCHICKENBITSCHICKEN"] = "0"

    #QURTK_chicken_bits_chicken_2nd
    try:
       cfg["QURTKCHICKENBITS2NDCHICKEN"] = chicken_el.find_child("chicken_2nd").value
    except AttributeError:
       cfg["QURTKCHICKENBITS2NDCHICKEN"] = "0"

    #QURTK_chicken_bits_chicken_3rd
    try:
       cfg["QURTKCHICKENBITS3RDCHICKEN"] = chicken_el.find_child("chicken_3rd").value
    except AttributeError:
       cfg["QURTKCHICKENBITS3RDCHICKEN"] = "0"


    supplement_rgdr_val= [0 for i in range(16)] 
    supplement_rgdr_revid= [0 for i in range(16)] 
    supplement_chicken_val= [0 for i in range(16)] 
    supplement_chicken_revid= [0 for i in range(16)]
    supplement_rgdr_no = 0
    supplement_chicken_no = 0

    if chicken_el is not None:
        supplement_rgdr_el = chicken_el.find_child("supplement_rgdr")
        if supplement_rgdr_el is not None:
            for el in supplement_rgdr_el.find_children("rgdr"):
                supplement_rgdr_val[supplement_rgdr_no] = int(el.value, 16)
                supplement_rgdr_revid[supplement_rgdr_no] = int(el.revid, 16)
                supplement_rgdr_no += 1
        supplement_chicken_el = chicken_el.find_child("supplement_chicken")
        if supplement_chicken_el is not None:
            for el in supplement_chicken_el.find_children("chicken"):
                supplement_chicken_val[supplement_chicken_no] = int(el.value, 16)
                supplement_chicken_revid[supplement_chicken_no] = int(el.revid, 16)
                supplement_chicken_no += 1

    str_supp_rgdr_val = hex(supplement_rgdr_val[0])
    str_supp_rgdr_revid = hex(supplement_rgdr_revid[0])
    count = 1
    while (count < 16):
        str_supp_rgdr_val += ("," + hex(supplement_rgdr_val[count]))
        str_supp_rgdr_revid += ("," + hex(supplement_rgdr_revid[count]))
        count += 1

    str_supp_chicken_val = hex(supplement_chicken_val[0])
    str_supp_chicken_revid = hex(supplement_chicken_revid[0])
    count = 1
    while (count < 16):
        str_supp_chicken_val += ("," + hex(supplement_chicken_val[count]))
        str_supp_chicken_revid += ("," + hex(supplement_chicken_revid[count]))
        count += 1

    cfg["QURTKSUPNORGDR"] = repr(supplement_rgdr_no)
    cfg["QURTKSUPRGDR"] = str_supp_rgdr_val
    cfg["QURTKSUPREVRGDR"] = str_supp_rgdr_revid

    cfg["QURTKSUPCHICKEN"] = str_supp_chicken_val
    cfg["QURTKSUPREVCHICKEN"] = str_supp_chicken_revid
    cfg["QURTKSUPNOCHICKEN"] = repr(supplement_chicken_no)

    return



def collect_machine_element(cfg):
    """Collect the attributes of the machine element."""
    machine_el = cfg.find_child("machine")

    if not machine_el:
        return
       
    #QURTK_timetest_port
    param = machine_el.find_child("timetest_port")
    if param is None:
        param_val = hex(0) #Default timetest port value is "0"
    else:    
        param_val = hex(param.value)
    cfg["TIMETESTPORT"] = param_val
    
    #QURTK_isdb_flag_imem_addr
    param = machine_el.find_child("isdb_imem")
    if param is None:
        param_val = "0x0" #Default isdb_imem address
    else:
        param_val = hex(param.addr)
    cfg["ISDBIMEMADDR"] = param_val
    
    #QURTK_int_usrpd_check
    param = machine_el.find_child("interrupt_mpd_privilege_check")
    if param is None or param.enabled == 'true':   # Default is "true"
        param_val = "1"
    elif param.enabled == 'false':
        param_val = "0"
    else:
        sys.exit ("!!! enabled value for interrupt_mpd_privilege_check must be either 'true' or 'false' \n")
    cfg["CHKUSRPDINT"] = param_val

    #QURTK_hvx_check_power_lock
    param = machine_el.find_child("check_hvx_power_for_locking")
    if param is None or param.enabled == 'true':   # Default is "true"
        param_val = "1"
    elif param.enabled == 'false':
        param_val = "0"
    else:
        sys.exit ("!!! enabled value for check_hvx_power_for_locking must be either 'true' or 'false' \n")
    cfg["CHKHVXPWRLOCK"] = param_val

    #QURTK_config_max_island_int
    island_int_num = 0
    str_island_int = ""
    param = machine_el.find_child("max_island_interrupts")
    if param is not None:
        island_int_num = int(param.value)
        if(island_int_num > 0):
            str_island_int = hex(0xFFFF)
            count = 1
            while (count < island_int_num):
                str_island_int += ("," + hex(0xFFFF))
                count = count + 1

    cfg["INTISLANDNUM"] =  repr(island_int_num)
    cfg["INTISLANDDATA"] = str_island_int

    #QURTK_livelock_data
    param = machine_el.find_child("livelock_exception")
    if param is None:
        param_val = "0x0"
    else:
        param_val = hex(param.pcycles)
    cfg["QURTKLIVELOCKDATA"] = param_val

    #QURTK_etm_to_q6etb
    str_q6etb = ""
    param = machine_el.find_child("etm_trace_to_q6etb")
    if param is None or param.enabled == 'false':
        param_val = "0"
    elif param.enabled == 'true':
        param_val = "1"
        str_q6etb = hex(0)
        count = 1
        while (count < 512):
            str_q6etb += ("," + hex(0))
            count = count + 1
    else:
        sys.exit ("!!! Enabled value for etm_trace_to_q6etb must be either 'true' or 'false' \n")
    cfg["ETMTOQ6ETB"] = param_val

    #qurt_q6etb_dump
    cfg["INTIQ6ETBDUMP"] = str_q6etb

    #QURTK_etm_cfg_base
    param = machine_el.find_child("etm_cfg_base")
    if param is None:
        param_val = hex(0)  #Default undefined ETM config base address
    else:
        param_val = hex(param.addr)
    cfg["ETMCFGBASEADDR"] = param_val

    #QURTK_etm_atid
    param = machine_el.find_child("etm_atid")
    if param is None:
        param_val = hex(0x24) 
    else:
        param_val = hex(param.value)
    cfg["ETMATIDVAL"] = param_val
    
    #QURTK_dmt_enabled
    param = machine_el.find_child("dynamic_multi_threading")
    if param is None or param.enabled == 'true':
        param_val = "1"
    elif param.enabled == 'false':
        param_val = "0"
    else:
        sys.exit ("!!! Enabled value for dynamic_multi_threading must be either 'true' or 'false' \n")
    cfg["DMT_ENABLED"] = param_val

    #QURTK_BQ_shared
    param = machine_el.find_child("bus_resources_shared")
    if param is None or param.enabled == 'true':
        param_val = "1"
    elif param.enabled == 'false':
        param_val = "0"
    else:
        sys.exit ("!!! Enabled value for bus_resources_shared must be either 'true' or 'false' \n")
    cfg["BQ_SHARED"] = param_val

    #QURTK_tcxo_intmask
    try:
       param = machine_el.find_child("tcxo")
    except AttributeError:
       param = None
    if param is None:
        param_val = hex(0xffffffff) #Default timetest port value is "0"
    else:    
        param_val = hex(param.intmask)
    cfg["TCXOINTMASK"] = param_val
    #cfg["TCXOINTMASK"] = machine_el.find_child("tcxo").intmask


    #Cache
    cache_el = machine_el. find_child("cache")

    cache_val = 1

    # HFI = 1, HFD = 0, HFIL2 = 1, HFDL2 = 0
	# L1ICP = 0x01, L1DCP = 0x01, L2CP = 0x01
    cache_val = 0x00150049
    usr_HFI_L1 = 2
    usr_HFI_L2 = 1
    usr_HFD_L1 = 0
    # L1 I-cache prefetch
    param = cache_el.find_child("l1_iprefetch")
    if param is not None:
        if param.enabled == "true":
            cache_val = cache_val | (1 << 16)
            try:
                usr_HFI_L1 = int(param.lines)
                if usr_HFI_L1 > 2:
                    sys.exit ("!!! Invalid L1 I-cache lines %s. \n" % (param.lines))
            except AttributeError:
                None
        else:
            cache_val = cache_val & ~(1 << 16)
    # L1 D-cache prefetch. Reserved
    param = cache_el.find_child("l1_dprefetch")
    if param is not None:
        if param.enabled == "true":
            cache_val = cache_val | (1 << 17)
            try:
                usr_HFD_L1 = int(param.lines)
                if usr_HFD_L1 > 3:
                    sys.exit ("!!! Invalid L1 D-cache prefetch %s. \n" % (param.lines))
            except AttributeError:
                None
        else:
            cache_val = cache_val & ~(1 << 17)
    # L2 I-cache prefetch
    param = cache_el.find_child("l2_iprefetch")
    if param is not None:
        if param.enabled == "true":
            cache_val = cache_val | (1 << 18)
            try:
                usr_HFI_L2 = int(param.lines)
                if (usr_HFI_L2 == 3) or (usr_HFI_L2 > 4):
                    sys.exit ("!!! Invalid L2 I-cache lines %s. \n" % (param.lines))
                if (usr_HFI_L2 == 4):
                    usr_HFI_L2 = 3
            except AttributeError:
                None
        else:    
            cache_val = cache_val & ~(1 << 18)
    # L2 D-cache prefetch.
    param = cache_el.find_child("l2_dprefetch")
    if param is not None:
        if param.enabled == "true":
            sys.exit ("!!! L2 D-cache enable not supported \n")
    # Only L1 I-cache and L2 I-cache can influence USR settings
    usr_val = (usr_HFI_L1 << 15) | (usr_HFI_L2 << 18) | (usr_HFD_L1 << 13) 
    #QURTK_ssr_default / QURTK_ccr_default
    cfg["USRATTR"] = hex(usr_val)

    #QURTK_ssr_default / QURTK_ccr_default
    cfg["CACHEATTR"] = hex(cache_val)

    #QURTK_l1_icache_size and QURTK_l2cache_size
    try:
        l1_ic_size = cache_el.find_child("l1_icache_size").value
    except AttributeError:
        l1_ic_size = 16

    l1_ic_size *= 1024   # Actual L1 Icache size in Bytes
    cfg["L1ICSIZE"] = ('%#X' % l1_ic_size)
    cfg["L2SIZE"] = "kb_"+cache_el.find_child("l2_size").value

    #QURTK_icache_enable
    try:
       icache_en = cache_el.find_child("icache_usage").enabled
    except AttributeError:
       icache_en = "true"
       
    if icache_en == "true":
       cfg["ICACHEENABLE"] = "ENABLED"
    else:
       cfg["ICACHEENABLE"] = "DISABLED"

    #QURTK_l2cache_nwa
    try:
       l2wa_val = cache_el.find_child("l2_write_allocate").enabled
    except AttributeError:
       l2wa_val = "true"
       
    if l2wa_val == "true":
       cfg["L2CACHENWA"] = "DISABLED"
    else:
       cfg["L2CACHENWA"] = "ENABLED"

    #QURTK_l2cache_wb
    try:
       writeback_val = cache_el.find_child("l2_writeback").enabled
    except AttributeError:
       writeback_val = "false"
       
    if writeback_val == "true":
       cfg["L2CACHEWB"] = "ENABLED"
    else:
       cfg["L2CACHEWB"] = "DISABLED"
    
    #QURTK_DC_partition 
    param = cache_el.find_child("l1_dpartition")
    if param is None:
        param_val = "full"
    else:    
        param_val = param.main
    cfg["L1DP"] = param_val

    #QURTK_IC_partition
    param = cache_el.find_child("l1_ipartition")
    if param is None:
        param_val = "full"
    else:    
        param_val = param.main
    cfg["L1IP"] = param_val

    #QURTK_L2_partition
    param = cache_el.find_child("l2_partition")
    if param is None:
        param_val = "full"
    else:    
        param_val = param.main
    cfg["L2CP"] = param_val
   
    #Chicken bits
    chicken_el = machine_el.find_child("chicken_bits")
    collect_chicken_bits_element(cfg, chicken_el)

    #Timer
    timer_el = machine_el.find_child("timer")
    try:
       str = hex(timer_el.find_child("base").value)
    except AttributeError:
       str = hex(machine_el.find_child("Hexagon_ss_base").value)
    #QDSP6_QTIMER_BASE
    cfg["QDSP6QTIMERBASE"] = str
    
    #QURT_timer_intno
    cfg["TIMERINTNO"] = timer_el.find_child("intno").value
    
    #QURT_timerIST_priority
    cfg["TIMERISTPRIORITY"] = timer_el.find_child("IST_priority").value    

    #QURT_timerIST_tcb_partition
    param = timer_el.find_child("IST_tcb_partition")
    if param is None:
        cfg["TIMERISTTCBPARTITION"] = "0"
    else:
        cfg["TIMERISTTCBPARTITION"] = param.value

    #qdsp6ss_base_addr
    cfg["QDSP6SSBASE"] = hex(machine_el.find_child("Hexagon_ss_base").value)
