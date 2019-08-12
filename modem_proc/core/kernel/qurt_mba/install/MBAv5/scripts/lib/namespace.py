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
A namespace for objects declared in the XML config file and used in
the object environment.

Namespaces are hierarchical and have POSIX-like semantics.
"""

import string
#from weaver import MergeError

class ObjectNameSpace:
    SEPARATOR = '/'

    def __init__(self, parent):
        # If parent is None, then this namespace is the top of the tree.
        if parent is None:
            parent = self
            root   = self
        else:
            root   = parent.get_root()

        self.root    = root
        self.parent  = parent
        self.symbols = {}

    def get_parent(self):
        """Return the parent of the namespace."""
        return self.parent

    def get_root(self):
        """Return the root of the namespace tree."""
        return self.root

    def add(self, name, object):
        """
        Add an object to the namespace under the given name if the
        name isn't already in use.
        """
        if self.symbols.has_key(name):
            raise MergeError, 'Object name "%s" already in use.' % name

        self.symbols[name] = object

    def add_namespace(self, name):
        """Add a new sub-namespace to this namespace."""
        new_namespace = ObjectNameSpace(parent = self)
        self.add(name, new_namespace)

        return new_namespace

    def namei(self, parts):
        """
        Recursively move the list of names, looking for the  target.

        Historical Note: This function is named after a similar
        function in the UNIX v6 kernel.
        """
        
        if len(parts) == 0:
            # If the name refers to a namespace, return the master cap
            # if present.
            if self.symbols.has_key('master'):
                return self.symbols['master']
            else:
                return None
        elif parts[0] == '.':
            return self.namei(parts[1:])
        elif parts[0] == '..':
            return self.get_parent().namei(parts[1:])
        elif self.symbols.has_key(parts[0]):
            obj = self.symbols[parts[0]]
            if isinstance(obj, ObjectNameSpace):
                return obj.namei(parts[1:])
            elif len(parts) == 1:
                return obj
            else:
                return None            
        else:
            return None

    def lookup(self, name):
        """
        Lookup a name in the namespace.  If found the object will be
        returned, otherwise None.
        """
        # Convert the string into a list of parts.  Absolute paths
        # have an empty string as the first element.
        parts = name.split(ObjectNameSpace.SEPARATOR)

        # If an absolute path, then search from the root of the
        # namespace, otherwise search from the current namespace.
        if parts[0] == '':
            start = self.get_root()
            parts = parts[1:]
        else:
            start = self
        
        return start.namei(parts)

    def abs_name(self, name):
        """
        Return the absolute path of a name in the current namespace.
        Returns None if the name does not exist.
        """
        if name != '.':
            if not self.symbols.has_key(name):
                return None

            path = [name]
        else:
            path = []

        node   = self
        parent = self.parent

        while parent is not node:
            for (dir_name, obj) in parent.symbols.items():
                if obj is node:
                    path = [dir_name] + path
                    break

            node = parent
            parent = node.parent

        return '/' + string.join(path, '/')
