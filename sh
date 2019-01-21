#!/bin/bash
#
# Copyright (C) 2020 Pig <pig.priv@gmail.com>
#
# Simple script to import/update kernel modules
# Version 0.3
#

# Colors for script
BOLD="\033[1m"
GRN="\033[01;32m"
RED="\033[01;31m"
RST="\033[0m"
YLW="\033[01;33m"

# Aliases
cai='git commit --amend --no-edit'
ci='git commit --no-edit'
ds='drivers/staging'
f='git fetch caf'
fn='FETCH_HEAD --no-edit'
mo='git merge --log --allow-unrelated-histories -s ours --no-commit FETCH_HEAD'
ms='git merge -X subtree'
os='opensource'
qc='qcom'
r='git read-tree --prefix'
rm='https://source.codeaurora.org/quic/la/platform/vendor'
rma='git remote add caf'
sa='git subtree add --prefix'
tp='techpack'
uf='-u FETCH_HEAD'
wl=$qc-$os/wlan

# Alias for echo to handle escape codes like colors
function echo() {
    command echo -e "$@"
}

# Prints a formatted header to point out what is being done to the user
function header() {
    if [[ -n ${2} ]]; then
        COLOR=${2}
    else
        COLOR=${RED}
    fi
    # shellcheck disable=SC2034
    echo "${COLOR}\n====$(for i in $(seq ${#1}); do echo "=\c"; done)====\n==  ${1}  ==\n====$(for i in $(seq ${#1}); do echo "=\c"; done)====\n${RST}"
}

# Prints an error in bold red
function die() {
    echo "${RED}${1}${RST}"
    [[ ${2} = "-h" ]] && ${0} -h
    exit 1
}

# Prints a statement in bold green
function success() {
    echo "${GRN}${1}${RST}"
    [[ -z ${2} ]] && echo
}

# Prints an info in bold yellow
function info() {
    echo
    echo "${YLW}${1}${RST}"
    [[ -z ${2} ]] && echo
}

# Read git cmd
function readcmd() {
    if [ $cmd = 's' ]; then
    $f/$mod $br && $sa=$dir caf/$mod $br && $cai
    elif [ $cmd = 'm' ]; then
    if [ $option = 'u' ]; then
      $f/$mod $br && $ms=$dir $fn
    else $f/$mod $br && $mo && $r=$dir $uf && $ci
    fi
    else die "Invalid target cmd, aborting!"
    fi
}

# Indicate module directories
function indicatemodir() {
if [ $num = '1' ]; then
    mod=qcacld-3.0
    elif [ $num = '2' ]; then
    mod=qca-wifi-host-cmn
    elif [ $num = '3' ]; then
    mod=fw-api
    elif [ $num = '4' ]; then
    mod=audio-kernel
    dir=$tp/audio
    elif [ $num = '5' ]; then
    mod=data-kernel
    dir=$tp/data
    elif [ $num = '6' ]; then
    mod=camera-kernel
    dir=$tp/camera
    else die "Invalid target input, aborting!"
fi

if [ $num -eq '1' ] || [ $num -eq '2' ] || [ $num -eq '3' ]; then
    dir=$ds/$mod
fi
}

# Add remote
function addremote() {
    if [ $num -eq '1' ] || [ $num -eq '2' ] || [ $num -eq '3' ]; then
    $rma/$mod $rm/$wl/$mod
    elif [ $num -eq '5' ]; then
    $rma/$mod $rm/$qc-$os/$mod
    elif [ $num -eq '4' ] || [ $num -eq '6' ] ; then
    $rma/$mod $rm/$os/$mod
    fi
}

# Initialize
function init() {
header "Available modules"
info "1.qcacld-3.0\n2.qca-wifi-host-cmn\n3.fw-api\n4.audio-kernel\n5.data-kernel\n6.camera-kernel"
read -p "Target kernel module: " num
case $num in 
    1 | 2 | 3 | 4 | 5 | 6)
    read -p "Target tag / branch: " br
    read -p "Import (i) / Update (u): " option
    if [ $option != u ]; then
    read -p "Target cmd: merge (m) subtree (s) " cmd
    else cmd=m
    fi
esac
}

# Import modules
function import() {
    addremote
    success "Add remote for target module ${mod} done."
    case $mod in
        qcacld-3.0 | qca-wifi-host-cmn | fw-api | audio-kernel | data-kernel | camera-kernel)
        readcmd
        success "Import from target ${br} for target ${mod} done."
    esac
}

# Update modules
function update() {
    case $mod in
        qcacld-3.0 | qca-wifi-host-cmn | fw-api | audio-kernel | data-kernel | camera-kernel)
        readcmd
        success "Update to target ${br} for target module ${mod} done."
    esac
}

# Process import or update
function process() {
    case $option in
        i)
        import
        ;;
        u)
        update
        ;;
        *)
        die "Invalid target option, aborting!"
        ;;
    esac
}

init
indicatemodir
process
