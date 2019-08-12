#! /usr/bin/python

'''
    To use this file to create a linker command script from within SCons, do something like this:

    import imp
    qurt_island_py_file = '%s/qurt-island-link.py' % QURT_SCRIPT_DIRECTORY
    qurtisland = imp.load_source('qurtisland', qurt_island_py_file)
    qurtisland.create_lcs(output_file_name, template_file_name, None, island_spec_file_name)
'''

def create_lcs(linker_script_name, template_script_name, full_file_names, *spec_file_list):
    '''
    Read in all of the named spec files and the template script.
    Write out the final linker script.
    If full_file_names is a non-empty list, then basenames which are retrieved from the spec_file_list
    are expanded to match any full names in the list.  If full_file_names is an empty list or None,
    then the names in the spec_file_list are not modified at all.  Example:
    If the spec file lists libfoobar.a, and if the full_file_names array contains an entry with the
    string '/the/path/to/libfoobar.a', then the full name is substituted for libfoobar.a when
    filling in the template.  This is intended so that test cases can run with a constant island
    spec file which contains only base names even if the linker command line contains full path names.
    '''

    if full_file_names:
        import os.path
        def cpath(s):
            for a in full_file_names:
                if os.path.basename(a) == s:
                    return a
            return '*%s' % s
    else:
        def cpath(s):
            return s

    island_sections = []
    island_specs = []

    for fname in spec_file_list:
        f = open(fname, 'r')
        for specline in f:
            temp = specline.split()
            if len(temp):
                if temp[0].startswith('#'):
                    # Treat it as a comment
                    pass
                elif len(temp) == 3:
                    if temp[2] != '*':
                        island_sections.append(temp)
                    else:
                        island_specs.append(temp)
                else:
                    raise Exception('Island spec file has bad format; each non-blank line must have 3 whitespace separated fields')
        f.close()

    # Get the sorted list of section names we've been handed.

    section_names = sorted(set([s[2] for s in island_sections]))

    # Read in the linker script file

    f = open(template_script_name, 'r')
    script_file = f.read()
    f.close()

    #
    # Define the anchor comments we're able to scan for and replace
    #
    anchors = '''
        ISLAND_EXCLUDE
        ISLAND_TEXT
        ISLAND_RODATA
        ISLAND_DATA
        ISLAND_RW_SECTIONS
        ISLAND_RO_SECTIONS
        QURT_ISLAND_UKERNEL
        QURT_MAIN_UKERNEL
        QURT_START
    '''
    regex_string = '(/\*\\s*(?:%s)\\s*\*/)' % '|'.join(anchors.split())

    import re
    tokens = re.split(regex_string, script_file)

    script_file = ''

    while len(tokens) >= 2:
        script_file += tokens[0]
        if 'QURT_ISLAND_UKERNEL' in tokens[1]:
            if island_sections or island_specs:
                # There's stuff going into the island
                script_file += ' .rela.ukernel : { *(.rela.*.ukernel.*) }\n'
                script_file += ' .ukernel.island : { *(*.ukernel.island) }'
            else:
                # There's nothing going into the island; don't put ukernel there
                pass
        elif 'QURT_MAIN_UKERNEL' in tokens[1]:
            if island_sections or island_specs:
                # The island ukernel stuff goes in the island, not here
                script_file += ' .ukernel.main : { *(*.ukernel.main) }'
            else:
                # The island ukernel stuff goes here, not in an unused island
                script_file += ' .rela.ukernel : { *(.rela.*.ukernel.*) }\n'
                script_file += ' .ukernel.main : { *(*.ukernel.main *.ukernel.island) }'
        elif 'QURT_START' in tokens[1]:
            script_file += ' .qurtstart : {}\n . = ADDR(.qurtstart);\n'
        elif 'ISLAND_EXCLUDE' in tokens[1]:
            script_file += ' EXCLUDE_FILE(\n'
            for i in island_specs:
                if i[0] == '*' and i[1] != '*':
                    # Single object file
                    script_file += '      %s\n' % cpath(i[1])
                elif i[1] == '*' and i[0] != '*':
                    # Single library
                    script_file += '      %s:\n' % cpath(i[0])
                elif i[1] != '*' and i[0] != '*':
                    # Object within a library
                    script_file += '      %s:%s\n' % (cpath(i[0]), i[1])
            script_file += '   ) '
        elif '_SECTIONS' in tokens[1]:
            constraint = 'ONLY_IF_%s' % tokens[1].split('_')[1]
            for nm in section_names:
                if (nm == '.text' or nm.startswith('.text.')):
                    # Treat these specially -- avoid the constraint
                    if constraint == 'ONLY_IF_RW':
                        continue
                    section_constraint = ''
                elif (nm == '.data' or nm.startswith('.data.')):
                    # Treat these specially -- avoid the constraint
                    if constraint == 'ONLY_IF_RO':
                        continue
                    section_constraint = ''
                else:
                    section_constraint = constraint
                script_file += '   %s.IS.island : %s {\n' % (nm, section_constraint)
                for i in island_sections:
                    if i[2] == nm:
                        if i[0] == '*' and i[1] != '*':
                            # Single object file
                            script_file += '      %s(%s)\n' % (cpath(i[1]), nm)
                        elif i[1] == '*' and i[0] != '*':
                            # Single library
                            script_file += '      %s:(%s)\n' % (cpath(i[0]), nm)
                        elif i[1] != '*' and i[0] != '*':
                            # Object within a library
                            script_file += '      %s:%s(%s)\n' % (cpath(i[0]), i[1], nm)
                        else:
                            # All objects, all libraries
                            script_file += '      *(%s)\n' % (nm)
                script_file += '   }\n'
        else:
            if 'ISLAND_TEXT' in tokens[1]:
                sections = ['.text .text.*']
            elif 'ISLAND_RODATA' in tokens[1]:
                sections = ['.rodata .rodata.*']
            elif 'ISLAND_DATA' in tokens[1]:
                sections = ['.bss .bss.* COMMON', '.data .data.*', '.sbss .sbss.* .scommon .scommon.*', '.sdata .sdata.*']
            script_file += ' /*START_ISLAND*/\n'
            for sec in sections:
                for i in island_specs:
                    if i[0] == '*' and i[1] != '*':
                        # Single object file
                        script_file += '      %s(%s)\n' % (cpath(i[1]),sec)
                    elif i[1] == '*' and i[0] != '*':
                        # Single library
                        script_file += '      %s:(%s)\n' % (cpath(i[0]),sec)
                    elif i[1] != '*' and i[0] != '*':
                        # Object within a library
                        script_file += '      %s:%s(%s)\n' % (cpath(i[0]), i[1], sec)
            script_file += ' /*END_ISLAND*/ '
        tokens = tokens[2:]

    script_file += tokens[0]

    f = open(linker_script_name, 'w')
    f.write(script_file)
    f.close()

# Each row of the parser spec is intended to extract some information from the linker command line
#  that we're passed.
# The elements of each row are (in order):
#  Short form of the linker option (or None if no short form)
#  Long form of the linker option (or None if no long form)
#  Key in our parser dictionary under which to accumulate matching options
#  Boolean for whether to strip the option or to pass it through
#   (If we're extending or modifying options, this should be True; if
#    we're only extracting info, this should be False)

parser_spec = [
    ['-o', '--output',      'output_file', False],    # Where the linker output goes.  We store temp files in that directory.
    ['-T', '--script',      'script_file', True ],    # The linker script.  We read this and create a modified version.
    [None, '--island-spec', 'ispec_file',  True ],    # The island spec.  Determines how we modify the linker script.
    ]

def qurt_island_fixup(original_args, passthru_args, parsed):
    for p in parser_spec:
        if not p[2] in parsed or len(parsed[p[2]]) != 1:
            raise Exception('qurt-island-link.py cannot parse arguments correctly')
        parsed[p[2]] = parsed[p[2]][0]

    import os.path
    outfile_name = os.path.join(os.path.dirname(parsed['output_file']),'qurt_island_link.lcs')

    create_lcs(outfile_name, parsed['script_file'], passthru_args, parsed['ispec_file'])

    passthru_args.insert(1, '--script=%s' % outfile_name)

    return passthru_args

def qurt_island_link(argv):
    def parse_match(key, strip, arg):
        if key:
            return (key, strip, arg)                                # A previous argument was still pending
        for p in parser_spec:
            if arg == p[0] or arg == p[1]:
                return (p[2], p[3], None)                           # Parsed it, but no string available yet
            if p[0] and arg.startswith(p[0]):
                return (p[2], p[3], arg.replace(p[0],'',1))         # Parsed it, return string
            if p[1] and arg.startswith(p[1]+'='):
                return (p[2], p[3], arg.replace(p[1]+'=','',1))     # Parsed it, return string
        return (None, None, None)

    try:
        original_args = argv[1:]
        passthru_args = []
        parsed = dict()
        strip = None
        key = None
        for arg in original_args:
            (key, strip, value) = parse_match(key, strip, arg)
            if value:
                parsed.setdefault(key,[]).append(value)
                key = None
            if not strip:
                passthru_args.append(arg)

        use_args = qurt_island_fixup(original_args, passthru_args, parsed)

        print ' '.join(use_args)

        import subprocess
        return subprocess.Popen(use_args).wait()
    except (SystemExit, KeyboardInterrupt):
        raise
    except Exception, err:
        import traceback
        traceback.print_exc()
    except:
        raise
    return 1

if __name__ == '__main__':
    import sys
    sys.exit(qurt_island_link(sys.argv))
