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
IST_bitmask_el = Element("IST_bitmask",
                         value = (str_attr, "optional"))
Timer_el = Element("timer",
                   QtimerBase_el,
                   Intno_el,
                   Priority_el,
                   Bitmask_el,
                   IST_priority_el,
                   IST_bitmask_el)
TimetestPort_el = Element("timetest_port",
                          value = (long_attr, "optional"))
Isdbmem_el = Element("isdb_imem",
                          addr = (long_attr, "required"))
DMT_el = Element("dynamic_multi_threading",
                 enabled = (str_attr, "required"));
BQ_el = Element("bus_resources_shared",
                 enabled = (str_attr, "required"));
ETM_cfg_base_el = Element("etm_cfg_base",
                     addr = (long_attr, "required"))
Rgdr_el = Element("rgdr",
                  value = (str_attr, "optional"))
Acc0_el = Element("acc0",
                  value = (str_attr, "optional"))
Acc1_el = Element("acc1",
                  value = (str_attr, "optional"))
Cbits_el = Element("chicken",
                  value = (str_attr, "optional"))
Chicken_el = Element("chicken_bits",
                     Rgdr_el,
                     Acc0_el,
                     Acc1_el,
                     Cbits_el)
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

L2_kilobytes_el = Element("l2_size",
                          value = (str_attr, "required"))

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
                    L2_kilobytes_el,
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
                     ETM_cfg_base_el,
                     DMT_el,
                     BQ_el,
                     Chicken_el,
                     Cache_el,
                     Cache_policy_el,
                     file = (str_attr, "optional"))

   
def collect_machine_element(parsed, ignore_name, asic):
    """Collect the attributes of the machine element."""
    machine_el = parsed.find_child("machine")

    if not machine_el:
        return
       
    symbols = {}
    
    #QURTK_timetest_port
    param = machine_el.find_child("timetest_port")
    if param is None:
        param_val = hex(0) #Default timetest port value is "0"
    else:    
        param_val = hex(param.value)
    symbols[re.compile("TIMETESTPORT")] = param_val
    
    #QURTK_isdb_flag_imem_addr
    param = machine_el.find_child("isdb_imem")
    if param is None:
        param_val = "0x0" #Default isdb_imem address
    else:
        param_val = hex(param.addr)
    symbols[re.compile("ISDBIMEMADDR")] = param_val

    #QURTK_etm_cfg_base
    param = machine_el.find_child("etm_cfg_base")
    if param is None:
        param_val = hex(0)  #Default undefined ETM config base address
    else:
        param_val = hex(param.addr)
    symbols[re.compile("ETMCFGBASEADDR")] = param_val
    
    #QURTK_dmt_enabled
    param = machine_el.find_child("dynamic_multi_threading")
    if param is None or param.enabled == 'true':
        param_val = "1"
    elif param.enabled == 'false':
        param_val = "0"
    else:
        sys.exit ("!!! Enabled value for dynamic_multi_threading must be either 'true' or 'false' \n")
    symbols[re.compile("DMT_ENABLED")] = param_val

    #QURTK_BQ_shared
    param = machine_el.find_child("bus_resources_shared")
    if param is None or param.enabled == 'true':
        param_val = "1"
    elif param.enabled == 'false':
        param_val = "0"
    else:
        sys.exit ("!!! Enabled value for bus_resources_shared must be either 'true' or 'false' \n")
    symbols[re.compile("BQ_SHARED")] = param_val

    #QURTK_tcxo_intmask
    try:
       param = machine_el.find_child("tcxo")
    except AttributeError:
       param = None
    if param is None:
        param_val = hex(0xffffffff) #Default timetest port value is "0"
    else:    
        param_val = hex(param.intmask)
    symbols[re.compile("TCXOINTMASK")] = param_val
    #symbols[re.compile("TCXOINTMASK")] = machine_el.find_child("tcxo").intmask


    #Cache
    cache_el = machine_el. find_child("cache")

    cache_val = 1

    # HFI = 1, HFD = 0, HFIL2 = 1, HFDL2 = 0
    cache_val = 0x00150000
    usr_HFI_L1 = 2
    usr_HFI_L2 = 1
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
            sys.exit ("!!! L1 D-cache enable not supported \n")
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
    usr_val = (usr_HFI_L1 << 15) | (usr_HFI_L2 << 18)
    # Set bit#25-bit#27 to 1 for FP exception
    usr_val = (7 << 25) | (usr_val)
    #QURTK_ssr_default / QURTK_ccr_default
    symbols[re.compile("USRATTR")] = hex(usr_val)

    #QURTK_ssr_default / QURTK_ccr_default
    symbols[re.compile("CACHEATTR")] = hex(cache_val)

    #QURTK_l2cache_size
    symbols[re.compile("L2SIZE")] = "kb_"+cache_el.find_child("l2_size").value

    #QURTK_l2cache_wb
    try:
       writeback_val = cache_el.find_child("l2_writeback").enabled
    except AttributeError:
       writeback_val = "false"
       
    if writeback_val == "true":
       symbols[re.compile("L2CACHEWB")] = "ENABLED"
    else:
       symbols[re.compile("L2CACHEWB")] = "DISABLED"
    
    #QURTK_DC_partition 
    param = cache_el.find_child("l1_dpartition")
    if param is None:
        param_val = "full"
    else:    
        param_val = param.main
    symbols[re.compile("L1DP")] = param_val

    #QURTK_IC_partition
    param = cache_el.find_child("l1_ipartition")
    if param is None:
        param_val = "full"
    else:    
        param_val = param.main
    symbols[re.compile("L1IP")] = param_val

    #QURTK_L2_partition
    param = cache_el.find_child("l2_partition")
    if param is None:
        param_val = "full"
    else:    
        param_val = param.main
    symbols[re.compile("L2CP")] = param_val
   
    #Chicken bits
    chicken_el = machine_el.find_child("chicken_bits")
    #QURTK_chicken_bits_rgdr
    try:
       symbols[re.compile("QURTKCHICKENBITSRGDR")] = chicken_el.find_child("rgdr").value
    except AttributeError:
       symbols[re.compile("QURTKCHICKENBITSRGDR")] = "0"
    
    #QURTK_chicken_bits_acc0
    try:
       symbols[re.compile("QURTKCHICKENBITSACC0")] =chicken_el.find_child("acc0").value
    except AttributeError:
       symbols[re.compile("QURTKCHICKENBITSACC0")] = "0"
    
    #QURTK_chicken_bits_acc1
    try:
       symbols[re.compile("QURTKCHICKENBITSACC1")] = chicken_el.find_child("acc1").value
    except AttributeError:
       symbols[re.compile("QURTKCHICKENBITSACC1")] = "0"
    
    #QURTK_chicken_bits_chicken
    try:
       symbols[re.compile("QURTKCHICKENBITSCHICKEN")] = chicken_el.find_child("chicken").value
    except AttributeError:
       symbols[re.compile("QURTKCHICKENBITSCHICKEN")] = "0"

    #Timer
    timer_el = machine_el.find_child("timer")
    try:
       str = hex(timer_el.find_child("base").value)
    except AttributeError:
       str = hex(machine_el.find_child("Hexagon_ss_base").value)
    #QDSP6_QTIMER_BASE
    symbols[re.compile("QDSP6QTIMERBASE")] = str
    
    #QURT_timer_intno
    symbols[re.compile("TIMERINTNO")] = timer_el.find_child("intno").value
    
    #QURT_timer_priority
    symbols[re.compile("TIMERPRIORITY")] = timer_el.find_child("priority").value
   
    #QURT_timer_bitmask
    try:
       symbols[re.compile("TIMERBITMASK")] = timer_el.find_child("bitmask").value
    except AttributeError:
       symbols[re.compile("TIMERBITMASK")] = "0xff"
    
    #QURT_timerIST_priority
    symbols[re.compile("TIMERISTPRIORITY")] = timer_el.find_child("IST_priority").value    
    
    #QURT_timerIST_bitmask
    try:
       symbols[re.compile("TIMERISTBITMASK")] = timer_el.find_child("IST_bitmask").value
    except AttributeError:
       symbols[re.compile("TIMERISTBITMASK")] = "0xff"
       
    #qdsp6ss_base_addr
    symbols[re.compile("QDSP6SSBASE")] = hex(machine_el.find_child("Hexagon_ss_base").value)

    return symbols
