#
#  File of general utility classes and functions
#   for QuRT Python scripting
#
#  Use one of the following to import:
#     import lib.qurt                   {Imports everything but with lib.qurt needed as a prefix}
#     from lib.qurt import *            {Imports everything, no prefix needed}
#     from lib.qurt import x,y,z        {Imports just the listed identifiers; replace x,y,z with actual identifiers}
#

if __name__ == '__main__':
    raise SystemExit('qurt.py is not an executable script')

class _QurtInf(object):
    #
    # Integer Infinities, usable as QurtInfinity and -QurtInfinity.
    # The intent is that these can be used where an integer is normally
    #  used, and that some basic arithmetic operations work between
    #  infinite values and themselves and with integers -- in particular
    #  comparison, subtraction, and sorting.
    # Note that this implementation differs from the IEEE concept of
    #  floating point infinities in one major way:  infinities of the
    #  same sign compare equal to each other and have a difference of
    #  integer zero.  QurtInfinity - QurtInfinity is equal to zero.
    #  (IEEE floating point defines Inf - Inf as being NaN.)
    #
    # Note that the class name is prepended with an underscore to
    #  indicate that it's not expected to be used directly by
    #  scripts.  There should generally only be two instances of
    #  this class in existence, and they are accessible as QurtInfinity
    #  (or +QurtInfinity) and -QurtInfinity.
    #
    def __init__(self, sign):
        self.sign = sign                            # sign = 1 or -1
    def __repr__(self):
        return self.repr_string
    def __cmp__(self,other):
        if isinstance(other,self.__class__):
            return (self.sign-other.sign)           # Comparison between infinities
        else:
            return self.sign                        # Comparison against non-infinity
    def __sub__(self,other):
        if self == other:
            return 0                                # Two infinities of the same sign have 0 difference
        else:
            return self                             # Subtracting anything else is a no-op
    def __rsub__(self,other):
        return -(self-other)                        # other-self; return -(self-other)
    def __pos__(self):
        return self                                 # +X returns X
    def __neg__(self):
        return self.negated                         # -X returns X.negated

QurtInfinity = _QurtInf(1)
QurtInfinity.repr_string = '+QurtInfinity'
QurtInfinity.negated = QurtInfinity.__class__(-1)   # Initialize the negative of +infinity
QurtInfinity.negated.repr_string = '-QurtInfinity'
QurtInfinity.negated.negated = QurtInfinity         # Initialize the negative of -infinity

class QurtAddressRange(object):
    #
    # Define range description.  This is a left boundary, a right boundary,
    #  and a set of attributes.  Attributes which are not present
    #  are returned as None.  Zero-length ranges evaluate as boolean False and
    #  have no attributes.  Non-empty ranges evaluate as boolean True and
    #  may have attributes.
    #
    __slots__ = ['left','right',
                 'addr','size',
                 'container',
                 'full','__dict__']                     # Keep these attributes out of __dict__

    def __getattr__(self, a):
        return None                                     # If an attribute is missing, it's None

    def __nonzero__(self):
        return (self.size > 0)                          # Allows testing non-zero range with "if range:"

    def __repr__(self):
        tmp = '%s(' % self.__class__.__name__
        if self.size > 0:
            leftrepr = '%r' % self.left
            rightrepr = '%r' % self.right
            try:
                leftrepr = '0x%X' % self.left
            except TypeError:
                pass
            try:
                rightrepr = '0x%X' % self.right
            except TypeError:
                pass
            tmp += 'left=%s,right=%s' % (leftrepr,rightrepr)
            for k in self.__dict__:
                tmp += ',%s=%r' % (k,self.__dict__[k])
        return tmp + ')'

    def __init__(self, addr=0, size=0, left=0, right=0, full=False, templates=None, container=None, **kw):
        if full:
            left,right = (-QurtInfinity,QurtInfinity)   # Range is full (infinite)
        elif right > left:
            pass                                        # Range already passed as left,right
        elif size > 0:
            left,right = (addr,addr+size)               # Range was (addr,size) and non-empty
        else:
            left,right = (0,0)                          # Range is empty

        self.left = left
        self.right = right
        self.addr = left
        self.size = right-left
        self.full = (left == -QurtInfinity and right == QurtInfinity)
        self.container = container

        if self.size > 0:
            if templates:
                for t in templates:
                    self.__dict__.update(t.__dict__)    # Add each template's attributes in order
            self.__dict__.update(kw)                    # Add new attributes passed with the creation request
            for k,v in self.__dict__.items():
                if v == None:
                    del self.__dict__[k]                # Delete attributes which are set to None

    def accepts(self, other):
        #
        # Called to check if self and other can be merged
        #
        # If they can be merged, should return the template
        #  array for merging the two.  Generally the template
        #  array will be either [self,other] to merge the
        #  attributes of other on top of self, or it will be
        #  [other] to replace the attributes.
        #
        # If they cannot be merged, throw an appropriate
        #  exception.
        #
        return [other]                                  # Base class behavior if not overridden is
                                                        #  to accept all merges, and to replace the
                                                        #  attributes rather than merging.

    def merge(self, other):
        #
        # Merge other over top of self
        #
        if self.left >= other.right:
            return                                      # No overlap
        if other.left >= self.right:
            return                                      # No overlap
        templates = self.accepts(other)
        newobjs = [self.__class__(left=self.left,
                                  right=other.left,
                                  templates=[self],
                                  container=self.container),
                   other.__class__(left=max(self.left, other.left),
                                   right=min(self.right, other.right),
                                   templates=templates,
                                   container=self.container),
                   self.__class__(left=other.right,
                                  right=self.right,
                                  templates=[self],
                                  container=self.container)]
        ix = self.container.ranges.index(self)
        self.container.ranges[ix:ix+1] = [obj for obj in newobjs if obj]

class QurtAddressSpace(object):
    #
    # Describe an entire address space.  An address space is basically
    #  represented by an array of QurtAddressRange objects (or objects
    #  derived from QurtAddressRange).  The individual range objects
    #  cover the entire range from address -QurtInfinity to +QurtInfinity,
    #  thus making no assumptions about the size of the address space.
    #  There are no gaps; undescribed or unfilled areas in an address
    #  space are filled with ranges with no attributes.
    #
    def __init__(self):
        self.ranges = [QurtAddressRange(full=True, container=self)]
    def __iadd__(self, other):
        for x in self.ranges[:]:
            x.merge(other)
        print ' ['
        for x in self.ranges:
            print x
        print ' ]'
        return self

def convert_size(s):
    #
    # Convert the string to an integer.
    # Permit suffixes of 'K', 'KB', 'M', 'MB', 'G', 'GB'
    #  which correspond to the appropriate multipliers.
    # The suffix matching is non-case-sensitive.
    #
    import re
    p = re.split('(?i)([KMG]B?)', s, 1)      # Split with regex that matches K, M, G with an optional B, case-insensitive
    shift_count = 0
    if len(p) > 1:
        if p[2]:                             # The size suffix didn't come at the end of the string
            raise ValueError('Improper size format: %s' % s)
        shift_count = {'K':10,'M':20,'G':30}[p[1][0]]
    return int(p[0],0) << shift_count
