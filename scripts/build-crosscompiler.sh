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

declare binutils_version="2.35"
declare gcc_version="10.2.0"

function print_usage {
    printf "Usage: $0 <sysroot_dir> <target_triplet>\n"
}

function die {
    local msg="${1}"

    printf "${msg}\n" >&2
    exit 1
}

function main() {
    if [[ "$#" -ne 2 ]]; then
        print_usage
        exit 1
    fi

    local sysroot_dir="$(realpath "${1}")"
    local target_triplet="${2}"

    # Create the sysroot directory
    rm -rf -- "${sysroot_dir}"
    mkdir -p -- "${sysroot_dir}"

    # Set some environment variables
    export PATH="${sysroot_dir}/bin:${PATH}"
    export MAKEFLAGS="$(printf -- "${MAKEFLAGS}\n" | grep -o -P -- "-j[0-9]+")"  # Only keep -jX

    # Change to the target directory
    pushd "${sysroot_dir}" > /dev/null

        # Create the usr/ and include/ directories
        ln -s . usr
        mkdir -p include/

        # Download and extract binutils

        printf "  WGET\t binutils-${binutils_version}.tar.xz\n"
        wget "https://ftp.gnu.org/gnu/binutils/binutils-${binutils_version}.tar.xz" -O binutils.tar.xz &> /dev/null

        printf "  TAR\t binutils-${binutils_version}.tar.xz\n"
        tar xf binutils.tar.xz

        printf "  MV\t binutils-${binutils_version} -> binutils\n"
        mv binutils-* binutils

        # Download and extract gcc

        printf "  WGET\t gcc-${gcc_version}.tar.xz\n"
        wget "https://ftp.gnu.org/gnu/gcc/gcc-${gcc_version}/gcc-${gcc_version}.tar.xz" -O gcc.tar.xz &> /dev/null

        printf "  TAR\t gcc-${gcc_version}.tar.xz\n"
        tar xf gcc.tar.xz

        printf "  MV\t gcc-${gcc_version}\n"
        mv gcc-* gcc

        # Build binutils

        printf "  MKDIR\t binutils/build\n"
        mkdir binutils/build

        printf "  MAKE\t binutils ${binutils_version}\n"
        pushd binutils/build > /dev/null
            ../configure \
                --target="${target_triplet}" \
                --prefix="${sysroot_dir}" \
                --disable-nls \
                --disable-werror \
            &> configure.log ||
                die "Configuration failed. See ${sysroot_dir}/binutils/configure.log for more information."
            make &> build.log ||
                die "Build failed. See ${sysroot_dir}/binutils/build.log for more information."
            make install &> install.log ||
                die "Install failed. See ${sysroot_dir}/binutils/install.log for more information."
        popd > /dev/null

        printf "  RM\t binutils-${binutils_version}.tar.xz\n"
        rm -rf binutils.tar.xz
        rm -rf binutils

        # Build gcc

        printf "  MKDIR\t gcc/build\n"
        mkdir gcc/build

        printf "  MAKE\t gcc ${gcc_version}\n"
        pushd gcc/build > /dev/null
            ../configure \
                --target="${target_triplet}" \
                --prefix="${sysroot_dir}" \
                --disable-nls \
                --enable-languages=c \
                --without-headers \
            &> configure.log ||
                die "Configuration failed. See ${sysroot_dir}/gcc/build/configure.log for more information."
            make all-gcc &> build.log ||
                die "Build failed. See ${sysroot_dir}/gcc/build/build.log for more information."
            make all-target-libgcc &>> build.log ||
                die "Build failed. See ${sysroot_dir}/gcc/build/build.log for more information."
            make install-gcc &> install.log ||
                die "Install failed. See ${sysroot_dir}/gcc/build/install.log for more information."
            make install-target-libgcc &>> install.log ||
                die "Install failed. See ${sysroot_dir}/gcc/build/install.log for more information."
        popd > /dev/null

        printf "  RM\t gcc-${gcc_version}.tar.xz\n"
        rm -rf gcc.tar.xz
        rm -rf gcc

        # Remove info/man
        rm -rf "${sysroot_dir}/share" "${sysroot_dir}/usr/share"

    popd > /dev/null
}

main "$@"
