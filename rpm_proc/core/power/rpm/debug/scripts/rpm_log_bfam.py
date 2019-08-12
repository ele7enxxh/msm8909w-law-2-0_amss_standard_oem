# vim: set ts=4 sw=4 noexpandtab smarttab autoindent:
from struct import unpack
from optparse import OptionParser
from math import log
import sys, re, os
import clock_parser
import ddr_parser
import pmic_parser
import bus_parser
import rpm_parser
import base_parser
import ocmem_parser
import railway_parser
import rbcpr_parser
import misc_parser
from target_data import *

#
# NPA-related pretty-printing
#
npa_client_names     = {}
npa_client_resources = {}
npa_resources        = {}

def npa_lookup(dictionary, handle):
    if handle in dictionary:
        return dictionary[handle]
    else:
        formatted_handle = '0x%0.8x' % handle
        print '  ***** WARNING ***** failed to match npa handle', formatted_handle
        return formatted_handle

def npa_not_loaded(handle):
    global npa_client_name_lookup
    global npa_client_resource_lookup
    global npa_resource_lookup

    print '  ***** WARNING ***** no NPA dump loaded.  All NPA name resolutions will fail.'
    print '  ***** WARNING ***** switching to silent NPA lookup failure mode.'
    npa_client_name_lookup     = lambda h: '0x%0.8x' % h
    npa_client_resource_lookup = lambda h: '0x%0.8x' % h
    npa_resource_lookup        = lambda h: '0x%0.8x' % h
    return '0x%0.8x' % handle

npa_client_name_lookup     = npa_not_loaded
npa_client_resource_lookup = npa_not_loaded
npa_resource_lookup        = npa_not_loaded

npa_client_regex   = re.compile(r'[^:]*:\s+npa_client\s+\(name: (?P<name>[^\)]*)\)\s+\(handle: (?P<handle>[^\)]+)\)\s+\(resource: (?P<resource>[^\)]+)\)')
npa_resource_regex = re.compile(r'[^:]*:\s+npa_resource\s+\(name: "(?P<name>[^"]+)"\)\s+\(handle: (?P<handle>[^\)]+)')

def parse_npa_dump(npa_dump_filename):
    global npa_client_name_lookup
    global npa_client_resource_lookup
    global npa_resource_lookup

    npa_dump = open(npa_dump_filename, 'r')
    for line in npa_dump.readlines():
        m = npa_client_regex.match(line)
        if m:
            npa_client_names[int(m.group('handle'), 16)] = m.group('name')
            npa_client_resources[int(m.group('handle'), 16)] = int(m.group('resource'), 16)
            continue
        m = npa_resource_regex.match(line)
        if m:
            npa_resources[int(m.group('handle'), 16)] = m.group('name')
            continue
    npa_dump.close()

    npa_client_name_lookup     = lambda h: npa_lookup(npa_client_names, h)
    npa_resource_lookup        = lambda h: npa_lookup(npa_resources, h)
    def client_resource_lookup(h):
        resource = npa_lookup(npa_client_resources, h)
        lookup_failed = isinstance(resource, str)
        return '<lookup failed>' if lookup_failed else npa_resource_lookup(resource)
    npa_client_resource_lookup = client_resource_lookup

#
# The parsing core
#

if __name__ == '__main__':
    # Get command line arguments
    parser = OptionParser()
    parser.add_option('-f', '--file', dest='filename',
                        help='RPM ULog file to parse', metavar='FILE')
    parser.add_option('-n', '--npa_dump', dest='npa_filename',
                        help='NPA dump to resolve NPA names from', metavar='FILE')
    parser.add_option('-r', '--raw_timestamp', dest='raw_timestamp',
                        help='Print timestamps as raw hex values', action='store_true', default=False)
    parser.add_option('-p', '--pretty_timestamp', dest='raw_timestamp',
                        help='Print timestamps as pretty floating point values', action='store_false')
    parser.add_option('-t', '--target', dest='target', default="8974",
                        help='What target was this RPM build taken from (chip number only, e.g. "8660" or "8960")')
    parser.add_option('-b', '--branch', dest='branch', default="RPM.BF.1.0-00000",
                        help='What branch/build is this taken from, "RPM.{family}.{major}.{minor}[-build number]", build number is optional')
    (options, args) = parser.parse_args()
    #parser_lookup()
    if not options.filename:
        parser.error('-f option is required')

    if options.npa_filename:
        parse_npa_dump(options.npa_filename)
    
    #parse branch
    if store_branch(options.branch):
        print 'Error: unable to parse branch, defaulting to RPM.BF.2.0-00000'
        store_branch("RPM.BF.2.0-00000")

    if options.target not in all_targets_data:
        print 'Error: unknown target %s' % options.target
        sys.exit(1)
    select_target(options.target)

    # Try to load data from the log file.
    try:
        f = open(options.filename)
        loglines = f.readlines()
        f.close()
    except:
        print 'Error loading log data from file:', sys.exc_info()[0]
        raise

    # Got data, parse it as well as we can.
    for line in loglines:
        # First try to unpack it into its components.
        try:
            assert '- ' == line[0:2]
            bytestring = ''.join(map(lambda x: chr(int(x, 16)), line[2:line.rfind(',')].split(', ')))
            message = list(unpack('<%iL' % (len(bytestring)/4), bytestring))

            timestamp = (message[1] << 32) | message[0]
            id = message[2]
            data = message[3:]
        except:
            print 'Error parsing message from logfile:', sys.exc_info()[0]

        # Then try to find a parser for it.
        if options.raw_timestamp:
            timestamp = '0x%0.16x' % (timestamp)
        else:
            timestamp = '%f' % (timestamp * (1 / 19200000.0))

        try:
            pretty_message = base_parser.parsers[id]().parse(data)
            print "%s: %s" % (timestamp, pretty_message)
        except:
            print 'Error parsing log message with timestamp = %s, id = %i, and data = %s -- %s' % (timestamp, id, data, sys.exc_info()[0])

