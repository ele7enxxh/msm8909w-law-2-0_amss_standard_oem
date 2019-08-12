import getopt, sys, os, re
import subprocess as sub
import binascii
from time import time

# win32file.SetFilePointer(op_fp, 1019215872, win32file.FILE_BEGIN)
# win32file.ReadFile(op_fp, 512)
# win32file.WriteFile(op_fp, temp2, None)
# op_fp = win32file.CreateFile(device_name, win32con.GENERIC_READ | win32con.GENERIC_WRITE, win32file.FILE_SHARE_WRITE | win32file.FILE_SHARE_READ, None, win32file.OPEN_EXISTING, win32file.FILE_ATTRIBUTE_NORMAL, None)
# win32file.CloseHandle(op_fp)

def usage():
    print """
Usage: python dd.py [OPTION...] [FILE]...
Emulates dd.

Examples:
  python dd.py --if=inputfile.bin --bs=512 --count=34 --skip=0 --of=dump.bin
  python dd.py --if=dump.bin --bs=512 --count=34 --seek=0 --of=outputfile.bin

  python dd.py --if=singleimage.bin --of=\\.\PHYSICALDRIVE1 --bs=512 --count=1595
  
 Options:

  -b, --bs                   block size
  -n, --ibs                  input block size
  -p, --obs                  input block size
  -i, --if                   Input file/device
  -o, --of                   Ouput file/device
  -c, --count                Count block size
  -s, --seek                 Skip obs-sized blocks at start of Output
  -k, --skip                 Skip ibs-sized blocks at start of Input
  -e, --cbs                  Convert block size
  -?, --help                 Display this help message
  -l, --list                 List detected devices/partitions

    """

def show_message(text):
    MSG_LENGTH = 80
    if len(text) == 0:
        return
    print "-"*MSG_LENGTH
    while len(text):
        print text[:MSG_LENGTH]
        text = text[MSG_LENGTH:]
    print "-"*MSG_LENGTH

def parse_unit(value):
    if not value is None:
        unit_pattern = re.compile(r'^\s*(\d+)(c|w|b|kB|K|MB|M|xM|GB|G){0,1}\s*$')
        multiplier_table = { "c": 1, "w": 2, "b": 512, "kB": 1000, "K": 1024, "MB": 1000*1000, "M": 1024*1024, "xM": 1024*1024, "GB": 1000*1000*1000, "G": 1024*1024*1024 }
        unit_match = unit_pattern.search(value)
        if not unit_match is None:
            value = int(unit_match.groups()[0])
            multiplier = unit_match.groups()[1]
            if not multiplier is None:
                if multiplier == "c":
                    value = value * 1
                elif multiplier == "w":
                    value = value * 2
                elif multiplier == "b":
                    value = value * 512
                elif multiplier == "kB":
                    value = value * 1000
                elif multiplier == "K":
                    value = value * 1024
                elif multiplier == "MB":
                    value = value * 1000 * 1000
                elif multiplier == "M" or multiplier == "xM":
                    value = value * 1024 * 1024
                elif multiplier == "GB":
                    value = value * 1000 * 1000 * 1000
                elif multiplier == "G":
                    value = value * 1024 * 1024 * 1024
        else:
            print "Error: could not parse unit %s." % value
            sys.exit(1)
    return value

def check_file_exists(filename, required_file=False):
    file_exists = (not filename is None) and os.path.isfile(filename) and os.path.getsize(filename) > 0
    if (not file_exists) and required_file:
        print "%s does not exist. Cannot continue without this file." % filename
        #clean_up()
        sys.exit(1)
    return file_exists

def total_size(device_fp):
	current_pos = device_fp.tell()
	device_fp.seek(0, os.SEEK_END)
	tot_size = device_fp.tell()
	device_fp.seek(current_pos)
	return tot_size

def clean_up():
    global if_fp, of_fp
    if not if_fp is None:
        if_fp.close()
    if not of_fp is None:
        of_fp.close()

def external_call(command, capture_output=True):
    errors = None
    output = None
    try:
        if capture_output:
            p = sub.Popen(command, stdout=sub.PIPE, stderr=sub.PIPE)
            output, errors = p.communicate()
        else:
            os.system(command)
    except Exception, e:
        print "Error executing command %s. %s" % (str(command), e)
        #clean_up()
        sys.exit(1)
    finally:
        #if not output is None:
        #    print "Result: %s" % output
        if (not errors is None) and (not errors == ""):
            print "Process stderr: %s" % errors
    return output

def detect_devices():
    if sys.platform.startswith('win'):
        response = external_call('wmic DISKDRIVE get DeviceID, MediaType, Model, Size')
    else:
        response = external_call(['cat', '/proc/partitions'])
    print response

def is_device(stream_name):
    #return stream_name.startswith('/dev/sd') or stream_name.startswith(r'\\.')
    return stream_name.startswith('/dev/sd') or (sys.platform.startswith('win') and stream_name.startswith(r'\\.'))

def get_device_size(device_name):
    if sys.platform.startswith('win'):
        device_size = force_device_size(device_name)
    else:
        pass
    return device_size
    
def force_device_size(device_name):
    response = external_call('wmic DISKDRIVE get DeviceID, SectorsPerTrack, Size, TotalCylinders, TotalHeads, TotalSectors, TotalTracks, TracksPerCylinder /FORMAT:csv')
    response = response.replace('\r', '').strip("\n").split("\n")[1:]
    response = [x.split(",") for x in response if device_name.lower() in x.lower()]
    if not len(response) == 1:
        print "Device not found in wmic"
        sys.exit(1)
    node, device_id, sectors_per_track, device_size, total_cylinders, total_heads, total_sectors, total_tracks, tracks_per_cylinder = response[0]
    sectors_per_track, device_size, total_cylinders, total_heads, total_sectors, total_tracks, tracks_per_cylinder = [long(x) for x in [sectors_per_track, device_size, total_cylinders, total_heads, total_sectors, total_tracks, tracks_per_cylinder]]
    #print [sectors_per_track, device_size, total_cylinders, total_heads, total_sectors, total_tracks, tracks_per_cylinder]
    if device_size == (total_sectors*512) and total_sectors == (total_cylinders * tracks_per_cylinder * sectors_per_track):
        lower_bound_sector = total_cylinders * tracks_per_cylinder * sectors_per_track
        upper_bound_sector = (total_cylinders+1) * tracks_per_cylinder * sectors_per_track
        fp = open(device_name, 'rb')
        seek_max = None
        try:
            for i in range(lower_bound_sector, upper_bound_sector):
                fp.seek(i * 512)
                fp.read(512)
        except:
            seek_max = i - 1
        if not seek_max is None:
            print "Max seek is %s" % str(seek_max)
            device_size = (seek_max+1)*512
            read_block_size = 512
            try:
                fp.seek(seek_max * 512)
                while seek_max*512 + read_block_size < upper_bound_sector*512:
                    fp.read(read_block_size)
                    read_block_size = read_block_size + 512
            except:
                device_size = seek_max*512 + (read_block_size-512)
        else:
            print "Seek max not found"
        fp.close()
    return device_size
        
def dd(if_fp, of_fp=None, bs="512", ibs="512", obs="512", count="0", seek="0", skip="0", cbs="0", verbose=False):
    bs = bs is None and "512" or bs
    ibs = ibs is None and "512" or ibs
    obs = obs is None and "512" or obs
    count = count is None and "0" or count
    seek = seek is None and "0" or seek
    skip = skip is None and "0" or skip
    cbs = cbs is None and "0" or cbs
    
    of_is_device = None
    of_fp_handle = None
    
    # accounting/metrics
    records_read = 0
    records_read_extra = 0
    records_written = 0
    records_written_extra = 0
    time_start = None
    time_end = None
    progress = None
    prev_progress_shown = 0
    
    bs = parse_unit(bs)
    ibs = parse_unit(ibs)
    obs = parse_unit(obs)
    count = parse_unit(count)
    seek = parse_unit(seek)
    skip = parse_unit(skip)
    cbs = parse_unit(cbs)
    
    count = count * bs
    seek = long(seek)
    skip = long(skip)
    read_buffer = []
    if_is_device = is_device(if_fp)
    
    if not of_fp is None:
        of_is_device = is_device(of_fp)
    
    if_fp_handle = open(if_fp, "rb")
    if_fp_handle.seek(skip * ibs)
    try:
        if_fp_handle.read(ibs)
    except:
        pass
    if_fp_handle.seek(skip * ibs)
    
    if not of_fp is None:
        if of_is_device:
            of_fp_handle = open(of_fp, "r+b", (1024 * 256))
        else:
            if os.path.exists(of_fp):
                show_message("WARNING: Existing file %s will be modified. New file will not be created" % of_fp)
                of_fp_handle = open(of_fp, "r+b")
            else:
                of_fp_handle = open(of_fp, "wb")
        if of_is_device:
            of_fp_handle.seek(seek * obs)
            try:
                of_fp_handle.read(obs)
            except:
                print "Error in reading from %s at %d" % (of_fp, seek * obs)
        of_fp_handle.seek(seek * obs)
    
    # this is the most important section which handles the reads and writes
    time_start = time()
    for bytes_read in range(0, count, ibs):
        #print "input cursor at %s" % str(if_fp_handle.tell())
        #print "output cursor at %s" % str(of_fp_handle.tell())
        buffer_string = if_fp_handle.read(ibs)
        if len(buffer_string) == ibs:
            records_read += 1
        else:
            records_read_extra += len(buffer_string)
        if count-bytes_read < ibs:
            buffer_string = buffer_string[:(count-bytes_read)]
        read_buffer.extend([x for x in buffer_string])
        
        if verbose:
            progress = (((1.0 * bytes_read)/count)*100)
            if progress-(progress%10)-prev_progress_shown > 10 or progress == 100:
                print (progress-(progress%10)), "% ",
                prev_progress_shown = (progress-(progress%10))
        
        for bytes_written in range(0, len(read_buffer), obs):
            if  obs <= len(read_buffer):
                buffer_string = ''.join(read_buffer[:obs])
                read_buffer = read_buffer[obs:]
                if of_fp is None:
                    print binascii.hexlify(buffer_string)
                else:
                    temp=of_fp_handle.tell()
                    try:
                        of_fp_handle.write(buffer_string)
                        records_written += 1
                    except:
                        print "Write failed for %s at %d" % (of_fp, temp)
                    temp=of_fp_handle.tell()
                    of_fp_handle.seek(temp)
                    #print "output cursor at %s" % str(of_fp_handle.tell())
                    #of_fp_handle.seek(seek * obs)
                    #of_fp_handle.read(512)
    
    # Write out any remaining data
    if len(read_buffer) > 0:
        records_written_extra += len(read_buffer)
        buffer_string = ''.join(read_buffer) + '\x00'*(obs-len(read_buffer))
        if of_fp is None:
            print binascii.hexlify(buffer_string)
        else:
            try:
                of_fp_handle.write(buffer_string)
            except:
                print "Write failed for %s at %d" % (of_fp, temp)
            temp=of_fp_handle.tell()
            of_fp_handle.seek(temp)
    
    time_end = time()
    if verbose:
        print "Transfer complete"
    
    if_fp_handle.close()
    
    if not of_fp is None:
        of_fp_handle.close()
    
    total_bytes = (records_read*ibs)+records_read_extra
    print "%d+%d records in" % (records_read, records_read_extra)
    print "%d+%d records out" % (records_written, records_written_extra)
    print "%d bytes copied, %0.3f s, %0.2f bytes/s" % (total_bytes, time_end-time_start, (total_bytes/(time_end-time_start)))

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "b:n:p:i:o:c:s:k:e:?lz:v", ["bs=", "ibs=", "obs=", "if=", "of=", "count=", "seek=", "skip=", "cbs=", "help", "list", "size=", "verbose"])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(1)
    
    bs = None
    ibs = None
    obs = None
    if_fp = None
    of_fp = None
    count = None
    seek = None
    skip = None
    cbs = None
    verbose = False
    
    for o, a in opts:
        if o in ("-b", "--bs"):
            bs = a
        elif o in ("-n", "--ibs"):
            ibs = a
        elif o in ("-p", "--obs"):
            obs = a
        elif o in ("-i", "--if"):
            if_fp = a
        elif o in ("-o", "--of"):
            of_fp = a
        elif o in ("-c", "--count"):
            count = a
        elif o in ("-s", "--seek"):
            seek = a
        elif o in ("-k", "--skip"):
            skip = a
        elif o in ("-e", "--cbs"):
            cbs = a
        elif o in ("-?", "--help"):
            usage()
            sys.exit()
        elif o in ("-v", "--verbose"):
            verbose = True
        elif o in ("-l", "--list"):
            detect_devices()
            sys.exit()
        elif o in ("-z", "--size"):
            if not is_device(a):
                print "Invalid device name %s." % a
            else:
                print "Device size: %s" % str(get_device_size(a))
            sys.exit()
        else:
            assert False, "unhandled option"
    
    if if_fp is None:
        print "No input to read from."
        usage()
        sys.exit(1)
    
    dd(if_fp=if_fp, bs=bs, ibs=ibs, obs=obs, count=count, seek=seek, skip=skip, cbs=cbs, of_fp=of_fp, verbose=verbose)

if __name__ == "__main__":
    main()
