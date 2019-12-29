#!/usr/bin/env bash

################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
##
################################################################################

set -e -u

function print_usage() {
	printf "Usage: $0 [OPTIONS] <kernel>\n"
	printf "\n"
	printf "\t-b <boot_args>               Set some additionnal args to give to the kernel.\n"
	printf "\t-o <file>                    Place the output in <file> (Default: poseidon.iso)\n"
	printf "\t-h                           Print this help menu\n"
	exit 1
}

function main() {
	declare boot_args=""
	declare iso="poseidon.iso"

	while getopts hb:o: FLAG; do
		case $FLAG in
			h)
				print_usage;;
			b)
				boot_args="$OPTARG";;
			o)
				iso="$OPTARG";;
			*)
				printf "Unknown option\n"
				print_usage
		esac
	done

	shift $(($OPTIND - 1))

	if [[ $# -ne 1 ]]; then
		print_usage
	fi

	declare kernel_path=$1
	declare kernel_name=$(basename $kernel_path)

	declare temp=$(mktemp -d)
	declare grub_output=$(mktemp)

	mkdir -p -- "$temp/boot/grub"
	cp -- "$kernel_path" "$temp/boot/$kernel_name"

	cat -- > "$temp/boot/grub/grub.cfg" << EOF
set timeout=0

menuentry "Poseidon" {
	multiboot2 /boot/$kernel_name $boot_args
}
EOF

	printf "  GRUB\t $(basename $iso)\n"

	declare grub="grub-mkrescue"
	if which "grub2-mkrescue" &> /dev/null; then
		grub="grub2-mkrescue"
	fi

	if ! ("$grub" -o "$iso" "$temp" &> "$grub_output"); then
		cat "$grub_output"
		exit 1
	fi

	rm -rf "$grub_output"
	rm -rf "$temp"
}

main $@
