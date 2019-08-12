#!/bin/bash

set -o errexit

usage() {
cat <<- EOF

    Sync OSS content from CAF.

    Usage:
        sync.sh --shallow-clone --quiet --jobs 8

    Options:
        a) Fetch all branches instead of just pulling the branch specified in
           the manifest. It is NOT recommended to specify this flag as it can
           substantially increase the sync time.
        q) Reduce verbosity
        s) Create a shallow clone with depth 1
        t) Fetch tags
        j) Number of projects to fetch simultaneously (defaults to 4)

EOF

exit 1
}

# Defaults

quiet=false
current_branch=true
shallow_clone=false
no_tags=true
repo_jobs=4
manifest_url='git://codeaurora.org/quic/la/platform/manifest'
manifest_file='caf_AU_LINUX_ANDROID_LAW.BR.2.0_RB1.07.01.01.177.017.xml'
manifest_branch='release'
repo_url='git://codeaurora.org/tools/repo.git'
repo_branch='caf-stable'

long_opts="fetch-all-branches,shallow-clone,quiet,fetch-tags,jobs:"

if ! getopts=$(getopt -o asqtj: -l $long_opts -- "$@"); then
   echo "Error processing options"
   usage
fi

eval set -- "$getopts"

while true; do
    case "$1" in
       -a|--fetch-all-branches) current_branch=false;;
       -s|--shallow-clone) shallow_clone=true;;
       -q|--quiet) quiet=true;;
       -t|--fetch-tags) no_tags=false;;
       -j|--jobs) repo_jobs=$2 ; shift;;
       --) shift ; break ;;
       *) echo "Error processing args -- unrecognized option $1" >&2
          usage;;
    esac
    shift
done

sync_args=()
init_args=()
build_tree="$(dirname $(readlink -f $0))"

if ! [[ $repo_jobs =~ ^[0-9]+$ ]] ; then
    echo 'ERROR: value passed to --jobs must be an integer'
    usage
fi

if $quiet; then
    sync_args+=('--quiet')
fi

if $current_branch; then
    sync_args+=('--current-branch')
fi

if $no_tags; then
    sync_args+=('--no-tags')
fi

if $shallow_clone; then
    if ! repo init --help | grep -q '\-\-depth'; then
        echo "ERROR: Current version of repo doesn't support shallow clone."
        echo "Please upgrade repo to use this option"
        usage
    fi
    init_args+=('--depth 1')
fi

cd "$build_tree"

repo init --manifest-url $manifest_url \
          --manifest-name $manifest_file \
          --manifest-branch $manifest_branch \
          --repo-url=$repo_url \
          --repo-branch=$repo_branch \
           ${init_args[@]}
repo sync --network-only --jobs=$repo_jobs ${sync_args[@]}
repo sync --detach --local-only --jobs=$repo_jobs ${sync_args[@]}

echo 'Sync Completed Successfully'
