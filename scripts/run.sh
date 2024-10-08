#!/usr/bin/env bash

################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2018-2024 - The Poseidon Authors
##
################################################################################

set -e -u

function print_usage {
    printf "Usage: $0 [OPTIONS] <iso>\n"
    printf "\n"
    printf "\t-d                    Debug mode\n"
    printf "\t-g                    Freeze on boot and connect to GDB\n"
    printf "\t-t                    Monitor mode\n"
    printf "\t-k                    Enables kvm (if available)\n"
    printf "\t-m <MB>               Memory (in MB) (Default: 512MB)\n"
    printf "\t-s <SMP>              Number of cores (Default: 1)\n"
    printf "\t-h                    Print this help menu\n"
}

function main() {

    # Check this script has ben launched the proper way.
    if [[ -z "${QEMU}" ]]; then
        printf "Please use `make run` instead of using this script directly.\n" >&2
        exit 1
    fi

    local qemu="${QEMU}"
    local qemu_args="${QEMU_ARGS} ${QEMU_EXTRA_ARGS+}"

    while getopts dgtkhm:s: FLAG; do
        case ${FLAG} in
            d) qemu_args+=" -d int,cpu_reset,guest_errors,unimp --no-reboot";;
            g) qemu_args+=" -s -S";;
            k) qemu_args+=" --enable-kvm";;
            t) qemu_args=$(printf -- "${qemu_args}\n" | sed s/serial/monitor/g);;
            m) qemu_args+=" -m ${OPTARG}";;
            s) qemu_args+=" -smp ${OPTARG}";;
            h) print_usage;;
            \?)
                printf "Unknown option\n"
                print_usage
        esac
    done

    shift $((${OPTIND} - 1))

    if [[ "$#" -ne 1 ]]; then
        print_usage
        exit 1
    fi

    local iso=${1}
    qemu_args+=" -cdrom $iso"

    if ! which "${qemu}" &> /dev/null; then
        printf "  ERROR\t ${qemu} not found. You must install qemu first.\n"
        exit 1
    fi

    printf "  qemu\t $(basename $iso)\n"
    "${qemu}" ${qemu_args}
}

main "$@"
