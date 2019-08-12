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

from ezxml import Element, long_attr, str_attr, size_attr
import re

Hthread_mask_el = Element("hthread_mask",
                      value = (str_attr,"required"))                 
Kernel_heap_size_el = Element("heap_size",
                      value = (size_attr,"optional"))
Max_threads_el = Element("max_threads",
                      value = (str_attr,"required"))
Max_threads_in_tcm_el = Element("max_threads_in_tcm",
                      value = (str_attr,"optional"))
Max_futexes_el = Element("max_futexes",
                      value = (str_attr,"required"))
Trace_mask_el = Element("trace_mask",
                      value = (str_attr,"required"))
Trace_size_el = Element("trace_size",
                      value = (size_attr,"required"))
Fastint_stack_el = Element("fastint_stack",
                      size = (str_attr,"optional"))
Kernel_tcm_size_el = Element("tcm_size",
                      value = (size_attr,"optional"))

                      
Kernel_el = Element("kernel",
                    Hthread_mask_el,
                    Kernel_heap_size_el,
                    Max_threads_el,
                    Max_threads_in_tcm_el,
                    Max_futexes_el,
                    Trace_mask_el,
                    Trace_size_el,
                    Fastint_stack_el, 
                    Kernel_tcm_size_el) 

def collect_kernel_element(parsed, ignore_name, asic):
    """Collect the attributes of the kernel element."""
    kernel_el = parsed.find_child("kernel")

    if not kernel_el:
        return
    
    symbols = {}
    
    #QURTK_hthread_startup
    symbols[re.compile("HTHREADMASK")] = kernel_el.find_child("hthread_mask").value

    try:
       symbols[re.compile(r"\bKERNELHEAPSIZE\b")] = hex(kernel_el.find_child("heap_size").value)
    except AttributeError:
       symbols[re.compile(r"\bKERNELHEAPSIZE\b")] = "0x00040000"
       
    symbols[re.compile(r"\bMAXTHREADS\b")] = kernel_el.find_child("max_threads").value
    try:
       symbols[re.compile("MAXTHREADSINTCM")] = kernel_el.find_child("max_threads_in_tcm").value
    except AttributeError:
       symbols[re.compile("MAXTHREADSINTCM")] = "0"
    symbols[re.compile("MAXFUTEXES")] = kernel_el.find_child("max_futexes").value    
    symbols[re.compile("TRACESIZE")] = hex(kernel_el.find_child("trace_size").value)
    symbols[re.compile("QURTKDEFAULTTRACEMASK")] =  kernel_el.find_child("trace_mask").value

    try:
       symbols[re.compile("FASTINTSTACKSIZE")] = kernel_el.find_child("fastint_stack").size
    except AttributeError:
       symbols[re.compile("FASTINTSTACKSIZE")] = "384"
    try:
       symbols[re.compile(r"\bTCMSIZE\b")] = hex(kernel_el.find_child("tcm_size").value)
    except AttributeError:
       symbols[re.compile(r"\bTCMSIZE\b")] = "32"
    
    return symbols

