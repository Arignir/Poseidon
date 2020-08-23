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

declare binutils_version="2.33.1"
declare gcc_version="9.2.0"

function print_usage {
    printf "Usage: $0 [OPTIONS] <cross_dir> <target_triplet>\n"
    printf "\n"
    printf "\t-h                    Print this help menu\n"
    exit 1
}

function die {
    printf "$1\n"
    exit 1
}

function main() {
    while getopts h FLAG; do
        case $FLAG in
            h) print_usage;;
            \?)
                printf "Unknown option\n"
                print_usage
        esac
    done

    shift $(($OPTIND - 1))

    if [[ $# -ne 2 ]]; then
        print_usage
    fi

    declare cross_dir="$1"
    declare target_triplet="$2"

    # Create the target directory
    rm -rf -- "$cross_dir" && mkdir -p -- "$cross_dir"

    # Create the installation directory
    mkdir -p -- "$cross_dir/sysroot"
    declare install_dir=$(realpath -- "$cross_dir/sysroot")

    # Set some environment variables
    export PATH="$install_dir/bin:$PATH"
    export MAKEFLAGS="$(echo -- "$MAKEFLAGS" | grep -o -- "-j[0-9]")"  # Only keep -jX

    # Change to the target directory
    pushd "$cross_dir" > /dev/null

        # Download and extract binutils

        printf "  WGET\t binutils-$binutils_version.tar.xz\n"
        wget "https://ftp.gnu.org/gnu/binutils/binutils-$binutils_version.tar.xz" -O binutils.tar.xz &> /dev/null

        printf "  TAR\t binutils-$binutils_version.tar.xz\n"
        tar xf binutils.tar.xz

        printf "  MV\t binutils-$binutils_version -> binutils\n"
        mv binutils-* binutils

        # Download and extract gcc

        printf "  WGET\t gcc-$gcc_version.tar.xz\n"
        wget "https://ftp.gnu.org/gnu/gcc/gcc-$gcc_version/gcc-$gcc_version.tar.xz" -O gcc.tar.xz &> /dev/null

        printf "  TAR\t gcc-$gcc_version.tar.xz\n"
        tar xf gcc.tar.xz

        printf "  MV\t gcc-$gcc_version\n"
        mv gcc-* gcc

        # Build binutils

        printf "  MKDIR\t binutils/build\n"
        mkdir binutils/build

        printf "  MAKE\t binutils $binutils_version\n"
        pushd "binutils/build" > /dev/null
            ../configure \
                --prefix="$install_dir" \
                --with-sysroot \
                --disable-nls \
                --disable-werror \
                --target="$target_triplet" \
            &> configure.log ||
                die "Configuration failed. See $cross_dir/binutils/configure.log for more information."
            make &> build.log ||
                die "Build failed. See $cross_dir/binutils/build.log for more information."
            make install &> install.log ||
                die "Install failed. See $cross_dir/binutils/install.log for more information."
        popd > /dev/null

        printf "  RM\t binutils-$binutils_version.tar.xz\n"
        rm -rf binutils.tar.xz

        # Build gcc

        printf "  MKDIR\t gcc/build\n"
        mkdir gcc/build

        printf "  MAKE\t gcc $gcc_version\n"
        pushd "gcc/build" > /dev/null
            ../configure \
                --prefix="$install_dir" \
                --disable-nls \
                --enable-languages=c \
                --without-headers \
                --target="$target_triplet" \
            &> configure.log ||
                die "Configuration failed. See $cross_dir/gcc/configure.log for more information."
            make all-gcc &> build.log ||
                die "Build failed. See $cross_dir/gcc/build.log for more information."
            make all-target-libgcc &>> build.log ||
                die "Build failed. See $cross_dir/gcc/build.log for more information."
            make install-gcc &> install.log ||
                die "Install failed. See $cross_dir/gcc/install.log for more information."
            make install-target-libgcc &>> install.log ||
                die "Install failed. See $cross_dir/gcc/install.log for more information."
        popd > /dev/null

        printf "  RM\t gcc-$gcc_version.tar.xz\n"
        rm -rf gcc.tar.xz

    popd > /dev/null
}

main $@
