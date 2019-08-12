import os
import sys

#get arguments
isOutput = 0;
isPaddr = 0;
guest_os = sys.argv[1];
if not os.path.isfile(guest_os):
    print("Guest OS image does not exist.");
    exit();
    
images = [];

images.append(guest_os);

for arg in sys.argv[2:len(sys.argv)]:
    if isOutput:
        output = arg;
        isOutput = 0;
    elif isPaddr:
        paddr = arg;
        isPaddr = 0;
    elif arg == "-p":
        isPaddr = 1;
    elif arg == "-o":
        isOutput = 1;
    else:
        if os.path.isfile(arg):
            images.append(arg);
        else:
            print(arg+" is not a valid file.");
            exit();

tools_root = os.getenv("Q6_TOOLS_ROOT"); #Use env variable if def
if tools_root == None:
    if os.system("which qdsp6-gcc") == 0: #Use PATH variable if avail
        tools_root = os.popen("which qdsp6-gcc").read();
        tools_root = tools_root.replace("/gnu/bin/qdsp6-gcc","");
    else:
        print "Q6_TOOLS_ROOT is not defined.";
        exit();

if (os.name == 'posix') : #linux
    obj_copy = tools_root+"/gnu/bin/qdsp6-objcopy";
    obj_dump = tools_root+"/gnu/bin/qdsp6-objdump";
    strip = tools_root+"/gnu/bin/qdsp6-strip";
    img_bld = sys.path[0]+"/qdsp6-image-build";
else:                     #windows
    obj_copy = tools_root+"\\gnu\\bin\\qdsp6-objcopy.exe";
    obj_dump = tools_root+"\\gnu\\bin\\qdsp6-objdump.exe";
    strip = tools_root+"\\gnu\\bin\\qdsp6-strip.exe";
    img_bld = sys.path[0]+"\\qdsp6-image-build.exe";
    guest_os = guest_os.replace("/", "\\");
    output = output.replace("/", "\\");
    for image in images:
        images[images.index(image)] = image.replace("/", "\\");

#address temp file
tmp_file = guest_os.replace("elf", "addr");

#strip symbols from QuRT os and app images and save in temp pbn files
for image in images:
    os.system(strip+" -s "+image+" -o "+image.replace("elf","pbn"));

#weave images into boot image with given physical address
img_bld_cmd = img_bld;
for image in images:
    img_bld_cmd += " "+image.replace("elf","pbn");
img_bld_cmd += " -p "+ paddr + " -o "+output+">"+tmp_file;
os.system(img_bld_cmd);

delta_paddr= [];
f=open(tmp_file, "r");
lines = f.readlines()
for line in lines:
    list = line.split()
    if len(list) >= 2:
        delta_paddr.append(str(int(list[0], 16) - int(list[1], 16))) 
f.close();

# delete all temp files
os.remove(guest_os.replace("elf", "addr"));
for image in images:
    os.remove(image.replace("elf", "pbn"));

count = 0;
# updated symbol files (ELFs) with correct physical addresses
for image in images:
    if len(delta_paddr) >= 2:
        start_addr = delta_paddr[count];
        os.system(obj_copy+" --change-start "+start_addr+" --change-section-lma .start"+
                start_addr+" --change-section-lma .init"+start_addr+
                " --change-section-lma .text"+start_addr+" --change-section-lma .fini"+
                start_addr+" --change-section-lma .rodata"+start_addr+
                " --change-section-lma .data"+start_addr+" --change-section-lma .eh_frame"+
                start_addr+" --change-section-lma .ctors"+start_addr+
                " --change-section-lma .dtors"+start_addr+" --change-section-lma .bss"+
                start_addr+" --change-section-lma .sdata"+start_addr+
                " --change-section-lma .sbss"+start_addr+" --change-section-lma .DEVCFG_DATA"+
                start_addr+" "+image);
        os.system(obj_dump+" -dClx "+image+" > "+image.replace("elf","dump"));
        count += 1;

