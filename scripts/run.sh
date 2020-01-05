#!/usr/bin/env bash

################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2020 - The Poseidon Authors
##
################################################################################

set -e -u

function print_usage {
	printf "Usage: $0 [OPTIONS] <iso>\n"
	printf "\n"
	printf "\t-a <arch>             Architecture (Default: i386)\n"
	printf "\t-d                    Debug mode\n"
	printf "\t-g                    Freeze on boot and connect to GDB\n"
	printf "\t-t                    Monitor mode\n"
	printf "\t-k                    Enables kvm (if available)\n"
	printf "\t-m <MB>               Memory (in MB) (Default: 512MB)\n"
	printf "\t-c <CPU>              Cpu to emulate\n"
	printf "\t-s <SMP>              Number of cores (Default: 1)\n"
	printf "\t-h                    Print this help menu\n"
	exit 1
}

# Qemu command and args
declare qemu="qemu-system-i386"
declare qemu_args=" -serial stdio -m 512"

while getopts dgtkhm:a:c:s: FLAG; do
	case $FLAG in
		d) qemu_args+=" -d int,cpu_reset,guest_errors,unimp --no-reboot";;
		g) qemu_args+=" -s -S";;
		k) qemu_args+=" --enable-kvm";;
		t) qemu_args=$(echo "$qemu_args" | sed s/serial/monitor/g);;
		m) qemu_args+=" -m $OPTARG";;
		s) qemu_args+=" -smp $OPTARG";;
		c) qemu_args+=" -cpu $OPTARG";;
		a)
			if [[ "$OPTARG" == "x86" ]]; then
				OPTARG="i386"
			fi
			qemu="qemu-system-$OPTARG";;
		h) print_usage;;
		\?)
			printf "Unknown option\n"
			print_usage
	esac
done

shift $(($OPTIND - 1))

if [[ $# -ne 1 ]]; then
	print_usage
fi

declare iso=$1
qemu_args+=" -cdrom $iso"

if ! which "$qemu" &> /dev/null; then
	printf "  ERROR\t $qemu not found. You must install QEMU first.\n"
	exit 1
fi

printf "  QEMU\t $(basename $iso)\n"
$qemu $qemu_args
