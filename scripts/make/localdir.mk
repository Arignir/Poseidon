################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2020 - The Poseidon Authors
##
################################################################################

# Find the local directory of the included makefile
GET_LOCAL_DIR	= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
