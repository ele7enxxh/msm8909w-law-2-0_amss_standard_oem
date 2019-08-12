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

"""The merge command is the main purpose of elf weaver. It provides a
way for merging many ELF files into one for the purpose of creating a
bootable image.

The merge_cmd function is the input from weaver.main. It parses command
line arguments and calls the merge() function which does most of the work.

The merge() command can also be called from another python library.
"""
import sys
from optparse import OptionParser
from parse_spec import parse_spec_xml, parse_spec_xml_string
from ezxml import EzXMLError, size_attr

from machine_xml import collect_machine_element
from kernel_xml import collect_kernel_element
from program_xml import collect_program_element
from physpool_xml import collect_physpool_element
from interrupt_xml import collect_interrupt_element

from parse_build_params import parse_build_params
from build_qurt_config import build_qurt_config

import namespace
import re

def collect_image_objects(parsed, ignore_name, 
                          namespace, build_options, tools_path, object_path):
    """
    Extract the information in the XML elements and shove them into
    various data structures for later memory layout and image
    processing.
    """

    (build_flags, asic) = parse_build_params()
    dict = collect_machine_element(parsed, ignore_name, asic)
    dict.update(collect_kernel_element(parsed, ignore_name, asic))
    dict.update(collect_program_element(parsed, ignore_name, asic))
    ret = collect_physpool_element(parsed, ignore_name, asic)
    if ret != None:
       dict.update(ret)
    else:
       dict[re.compile("PHYSPOOLS")] = ''
    dict.update(collect_interrupt_element(parsed, ignore_name, asic))
    
    if build_options != None:
      build_options = build_options + " " + build_flags
    else:
      build_options = build_flags
 
    build_qurt_config(dict, build_options, asic, tools_path, object_path)

def merge(spec_file, options):

    name_space = namespace.ObjectNameSpace(None)

    # parsed is an in memory tree of Element Objects
    # which is created from spec_file
    if options.spec_is_string:
        parsed = parse_spec_xml_string(spec_file)
    else:
        parsed = parse_spec_xml(spec_file)

    collect_image_objects(parsed, options.ignore_name,
                          name_space, options.build_options, options.tools_path, options.object_path)

def merge_cmd(args):
    #import pdb; pdb.set_trace()
    """Merge command call from main. This parses command line
    arguments and calls merge, which does all the main work."""
    parser = OptionParser("%prog update [options] specfile", add_help_option=0)
    parser.add_option("-H", "--help", action="help")
    parser.add_option("-o", "--output", dest="object_path", metavar="FILE",
                      help="Specify the binary file output path")
    parser.add_option('-i', "--ignore", dest="ignore_name",
                      action="store", default="^$",
                      help="A regex specifying programs to be ignored.")
    parser.add_option('-S', "--string", dest="spec_is_string",
                      action="store_true",
                      help="Treat the specfile argument as an XML string.")
    parser.add_option('-B', "--build_options", dest="build_options",
                      help="Add further compiler options.")
    parser.add_option('-T', "--tools_path", dest="tools_path",
                      help="Specify the tools path")

    (options, args) = parser.parse_args(args)

#    options.kernel_heap_size = size_attr(options.kernel_heap_size)
    options.ignore_name = re.compile(options.ignore_name)

#    if options.output_file is None and not options.last_phys: 
#        parser.error("Output must be specified")
#    if len(args) != 1:
#        parser.error("Expecting a spec file.")
    try:
        # During elfweaver-extensions development, verify the document.
#         if not options.spec_is_string:
#             import os
#             xmllint = os.system('xmllint --path "tools/pyelf/dtd ../../dtd ../dtd" -o /dev/null --valid %s' % args[0])
#             if xmllint != 0:
#                 return xmllint

        spec_file = args[0]
        merge(spec_file, options)        
    except EzXMLError, text:
        print >> sys.stderr, text
        sys.exit(1)
#    except MergeError, text:
#        print >> sys.stderr, 'Error: %s' % text
#        sys.exit(1)

    return 0

