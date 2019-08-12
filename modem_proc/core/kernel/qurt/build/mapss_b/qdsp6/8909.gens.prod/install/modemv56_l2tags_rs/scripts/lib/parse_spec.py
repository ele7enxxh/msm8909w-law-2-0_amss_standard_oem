from ezxml import Element

def c_stringify(s):
    # Prepend the string with its own length encoded
    #  as a 32-bit integer
    import struct
    s = struct.pack('<L', len(s)) + s
    col = 0
    ret = ''
    pre = ''
    for c in s:
        o = ord(c)
        if c == '\n':
            e = '\\n'               # Encode newline as \n
        elif o < 32 or o > 126:
            e = '\\%03o' % o        # Encode other non-printables as \<octal>
        elif c in r'\"':
            e = '\\' + c            # Encode backslash and double quote as \<char>
        else:
            e = c                   # Other printable characters are used directly
        if col+len(e) > 71:
            ret += '"\n'            # Terminate current line
            col = 0
        if col == 0:
            ret += pre+'"'          # Start new line
            col = len(pre)+1
        ret += e                    # Add encoded character
        col += len(e)
        if c == '\n':
            col = 80                # If we just added a new-line, force line break
        pre = '   '
    if col > 0:
        ret += '"'
    return (ret,len(s))

class QurtImageParse(Element):

    # Our top level wrapper around the Element class.
    # This allows us to selectively modify the behavior
    #  of the top level of the parser, and allows us
    #  to do arbitrary reads and writes of the raw XML
    #  parse data.

    def __init__(self,cfg):
        from machine_xml  import Machine_el
        from kernel_xml   import Kernel_el
        from physpool_xml import PhysicalPool_el
        from program_xml  import Program_el

        Element.__init__(self,
                         'image',
                         Machine_el,
                         Kernel_el,
                         PhysicalPool_el,
                         Program_el)

        self.cfg = cfg

    # Our walkdom() function gets control after the
    #  raw XML has been parsed but before we've done
    #  any specific interpretation of the XML.
    # This is the spot where we can do out-of-band
    #  parsing or modifying of the XML contents.
    # For now, what we do is pretty simple...  If
    #  no <build> element is found at the top level,
    #  we do nothing special.
    # If one or more <build> elements are found at
    #  the top level, we create a string representation
    #  for the XML file <build> elements in C syntax and store
    #  it along with its length into these attributes:
    #   self.cfg['XMLCSTRING']
    #   self.cfg['XMLLENGTH']
    #  After creating that, we strip the <build>
    #  elements from the XML tree.  It is expected
    #  that all parsing of <build> elements will
    #  be done at a later point by qurt-image-build.py.

    def walkdom(self, el):

        children = el.childNodes

        self.cfg.build_elements = []

        cstr = '<image>'

        for child in children:
            if child.nodeType == child.ELEMENT_NODE:
                if child.tagName == 'build':
                    if cstr == '':
                        cstr = '<image>'
                    cstr += child.toxml('utf-8')
                    el.removeChild(child)
                    self.cfg.build_elements.append(child)

        if cstr:
            (cstr, len) = c_stringify(cstr+'</image>')
        else:
            (cstr, len) = ('""', 0)

        # Double all backslashes in cstr.  We don't want re.sub()
        #  to treat them specially

        cstr = '\\\\'.join(cstr.split('\\'))

        self.cfg['XMLCSTRING'] = cstr
        self.cfg['XMLLENGTH'] = '%u' % len

        return Element.walkdom(self, el)

    # Our do_parse() function is the "public" method
    #  that the upper level calls which actually
    #  does the parsing.  All of the needed inputs
    #  are attached to self.cfg already.

    def do_parse(self):
        s = self.cfg.spec_file
        if not self.cfg.spec_is_string:
            f = open(s, 'rb')
            s = f.read()
            f.close()
        self.cfg.parsed = self.parse_xml_str(s)

