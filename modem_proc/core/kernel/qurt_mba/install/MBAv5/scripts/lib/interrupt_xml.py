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

Irq_el = Element("irq",
                 num = (str_attr, "required"),
                 trigger = (str_attr, "required"),
                 polarity = (str_attr, "required"),
                 l2base = (str_attr, "optional"),
                 l2trigger = (str_attr, "optional"),
                 l2polarity = (str_attr, "optional"))
Interrupt_el = Element("interrupt",
                       Irq_el)

def collect_v2v3_interrupt_elements (int_el):
    v2_base_string = ["0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0"]
    v2_type_string = ["0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0"]
    v2_polarity_string = ["0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0"]
    str_base = ""
    str_type = ""
    str_polarity = ""

    iel = 0xffffffff
    iahl = 0xffffffff
    
    int_symbols = {}

    for el in int_el.find_children("irq"):
        el_no = int(el.num)
        irq_val = 1       
        irq_val = irq_val << (31 - int(el.num))
        if el.trigger == "edge":
            iel = iel | irq_val
        else:
            iel = ~(~iel | irq_val)
        if el.polarity == "low":
            iahl = ~(~iahl | irq_val)
        else:
            iahl = iahl | irq_val                   

        try:
            v2_base_string[int(el.num)] = el.l2base
        except AttributeError:
            wot=0   
      
        try:
            v2_type_string[int(el.num)] = el.l2trigger
        except AttributeError:
            wot=0   
      
        try: 
            v2_polarity_string[el_no] = el.l2polarity
        except AttributeError:
            wot=0
    
    count = 0
    str_base= v2_base_string[count]
    str_polarity = v2_polarity_string[count]
    str_type = v2_type_string[count]
    count = 1
    while (count < 32):
        str_base += ("," + v2_base_string[count])
        str_polarity += ("," + v2_polarity_string[count])
        str_type += ("," + v2_type_string[count])
        count = count + 1

    int_symbols[re.compile("QURTKSIRCBASES")] = str_base
    int_symbols[re.compile("QURTKSIRCTYPE")] = str_type
    int_symbols[re.compile("QURTKSIRCPOLARITY")] = str_polarity
    int_symbols[re.compile("QURTKIEL")] = hex(iel)
    int_symbols[re.compile("QURTKIAHL")] = hex(iahl)

    return int_symbols

def collect_v4_interrupt_elements (int_el):
    """Collect the attributes of interrupt element for v4."""
    
    v4_type= [0xffffffff, 0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff]
    v4_polarity = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
    str_base = ""
    str_type = ""
    str_polarity = ""

    iel = 0xffffffff
    iahl = 0xffffffff
    
    
    int_symbols = {}

    for el in int_el.find_children("irq"):
        el_no = int(el.num)
      
        element_no = el_no/32
        shift_no = el_no%32
        trigger = v4_type[element_no]         
        irq_val = 1
        if el.trigger == "edge":
            trigger = trigger | (irq_val << shift_no)
        else:
            trigger = ~(~trigger | (irq_val << shift_no))          
        v4_type[element_no] = trigger
      
        polarity = v4_polarity[element_no]
        if el.polarity == "low":
            polarity = polarity | (irq_val << shift_no)
        else:
            polarity = ~(~polarity | (irq_val << shift_no))
        v4_polarity[element_no] = polarity
    
    count = 0
    str_base = "0"
    str_type = hex(v4_type[count])
    str_polarity = hex(v4_polarity[count])
    count = 1
    while (count < 32):
	str_base += ("," + "0")
        str_type += ("," + hex(v4_type[count]))
        str_polarity += ("," + hex(v4_polarity[count]))
        count = count + 1
          
    #import pdb; pdb.set_trace()
    int_symbols[re.compile("QURTKSIRCBASES")] = str_base
    int_symbols[re.compile("QURTKSIRCTYPE")] = str_type
    int_symbols[re.compile("QURTKSIRCPOLARITY")] = str_polarity
    int_symbols[re.compile("QURTKIEL")] = hex(iel)
    int_symbols[re.compile("QURTKIAHL")] = hex(iahl)

    return int_symbols

def collect_v5_interrupt_elements (int_el):
    """Collect the attributes of interrupt element for v5."""
    
    v5_type= [0xffffffff, 0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff]
    str_base = ""
    str_type = ""

    iel = 0xffffffff
    iahl = 0xffffffff
    
    
    int_symbols = {}

    for el in int_el.find_children("irq"):
        el_no = int(el.num)
      
        element_no = el_no/32
        shift_no = el_no%32
        trigger = v5_type[element_no]         
        irq_val = 1
        if el.trigger == "edge":
            trigger = trigger | (irq_val << shift_no)
        else:
            trigger = ~(~trigger | (irq_val << shift_no))          
        v5_type[element_no] = trigger
    
    count = 0
    str_base = "0"
    str_type = hex(v5_type[count])
    count = 1
    while (count < 32):
	str_base += ("," + "0")
        str_type += ("," + hex(v5_type[count]))
        count = count + 1
          
    #import pdb; pdb.set_trace()
    int_symbols[re.compile("QURTKSIRCBASES")] = str_base
    int_symbols[re.compile("QURTKSIRCTYPE")] = str_type
    int_symbols[re.compile("QURTKIEL")] = hex(iel)
    int_symbols[re.compile("QURTKIAHL")] = hex(iahl)

    return int_symbols

def collect_interrupt_element(parsed, ignore_name, asic):
    """Collect the attributes of the interrupt element."""
    
    machine_el = parsed.find_child("machine")
    if not machine_el:
        return                       
    
    interrupt_el = machine_el.find_child("interrupt")
    if not interrupt_el:
        return  

    symbols = {}

    if asic == "v4":
        symbols = collect_v4_interrupt_elements(interrupt_el)
    elif asic == "v5":
        symbols = collect_v5_interrupt_elements(interrupt_el)
    else:
        symbols = collect_v2v3_interrupt_elements(interrupt_el)
    
    max_int_val = 0
    for el in interrupt_el.find_children("irq"):  
        if int(el.num) > max_int_val:
            max_int_val = int(el.num)

    symbols[re.compile("MAXINT")] = repr(max_int_val)
    
    return symbols
