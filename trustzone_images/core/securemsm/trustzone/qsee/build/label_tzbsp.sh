#!/bin/bash

# A script to automate labeling trustzone.

echo_red()
{
    printf "\033[0;31m$@\033[0m\n"
}

USAGE="$0 <label_tzbsp> --amajor=<major> --aminor=<minor> --aedit=<edit> \
                        --cmajor=<major> --cminor=<minor> --aedit=<cedit>"

if [ $# -ge 1 ]; then
   while (( "$#" )); do
   case "$1" in
      --amajor*) AMAJOR=$(echo $1 | cut -f2 -d '=' | xargs printf "%02d") ;;
      --aminor*) AMINOR=$(echo $1 | cut -f2 -d '=' | xargs printf "%02d") ;;
      --aedit*) AEDIT=$(echo $1 | cut -f2 -d '='   | xargs printf "%02d") ;;
      --cmajor*) CMAJOR=$(echo $1 | cut -f2 -d '=' | xargs printf "%02d") ;;
      --cminor*) CMINOR=$(echo $1 | cut -f2 -d '=' | xargs printf "%02d") ;;
      --cedit*) CEDIT=$(echo $1 | cut -f2 -d '='   | xargs printf "%02d") ;;
      *) echo $USAGE; exit ;;
   esac
   shift
   done
fi

let INC=0
LAST_API_LABEL=$(p4 labels | grep -e VU_CORE_SME_TZBSP_API[\.0-9] | sort | tail -n1 | cut -d ' ' -f2)
[ -z "$AMAJOR" ] && AMAJOR=$(echo $LAST_API_LABEL | cut -d '.' -f2)
[ -z "$AMINOR" ] && AMINOR=$(echo $LAST_API_LABEL | cut -d '.' -f3)
[ -z "$AEDIT" ] && AEDIT=$(echo $LAST_API_LABEL | cut -d '.' -f4) && INC=1
let ZEROPAD=$AEDIT+$INC
LAEDIT=$(printf "%02d" $ZEROPAD);

let INC=0
LAST_CORE_LABEL=$(p4 labels | grep -e VU_CORE_SME_TZBSP[\.0-9] | sort | tail -n1 | cut -d ' ' -f2)
[ -z "$CMAJOR" ] && CMAJOR=$(echo $LAST_CORE_LABEL | cut -d '.' -f2)
[ -z "$CMINOR" ] && CMINOR=$(echo $LAST_CORE_LABEL | cut -d '.' -f3)
[ -z "$CEDIT" ] && CEDIT=$(echo $LAST_CORE_LABEL | cut -d '.' -f4) && INC=1
let ZEROPAD=$CEDIT+$INC
LCEDIT=$(printf "%02d" $ZEROPAD);

echo -n "Do you want to create label VU_CORE_SME_TZBSP.$CMAJOR.$CMINOR.$LCEDIT [y/n]?"
read inp
match=$(expr match "$inp" '[nN]')
[ $match -eq 1 ] && echo_red "Label aborted!\n" && exit 1

echo -n "Do you want to create label VU_CORE_SME_TZBSP_API.$AMAJOR.$AMINOR.$LAEDIT [y/n]?"
read inp
match=$(expr match "$inp" '[nN]')
[ $match -eq 1 ] && echo_red "Label aborted!\n" && exit 1

p4 tag -l VU_CORE_SME_TZBSP.$CMAJOR.$CMINOR.$LCEDIT //source/qcom/qct/core/securemsm/TrustZone/main/latest/tzbsp/...
p4 tag -l VU_CORE_SME_TZBSP_API.$AMAJOR.$AMINOR.$LAEDIT //source/qcom/qct/core/api/securemsm/main/latest/tzbsp/...

# Here for now, so we know how to automate checkins. May remove later.
#p4 change -o > .temp_p4_changes
#sed -i 's/<.*>//g'  .temp_text
#sed -i 's/^New Features:/This is an automated file/g'
#sed -i 's/^Fixes:/This is an automated file/g'
#p4 change -i < .temp_p4_changes
