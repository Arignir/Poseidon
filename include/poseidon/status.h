/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Kernel error management.
*/

#pragma once

#include <limits.h>

/*
** Kernel error management.
**
** This enum contains all the possible error-codes a kernel function can return.
** `OK` is zero and error values are strictly positive numbers.
**
** Feel free to complete it according your needs.
*/
enum status {
    OK = 0,

    /* Internal errors */
    ERR_INTERNAL,           // Internal / unspecified error. Keep this one for extrem cases.
    ERR_NOT_IMPLEMENTED,    // Operation is not implemented
    ERR_NOT_SUPPORTED,      // Operation is not supported
    ERR_OUT_OF_MEMORY,      // Operation couldn't complete because it went out of memory
    ERR_INTERRUPTED,        // Operation got interrupted

    /* Parameters errors */
    ERR_INVALID_ARGS,       // Invalid arguments
    ERR_OUT_OF_RANGE,       // One of the argument was outside the valid range for this operation
    ERR_BAD_HANDLER,        // The operation is missing a valid handler or callback to call

    /* State errors */
    ERR_BAD_STATE,          //  Operation couldn't complete because it's current state wouldn't allow the operation to complete
                            //  (eg: pre-conditions not satisfied)
    ERR_TIMED_OUT,          // The operation timed-out
    ERR_CANCELED,           // The operation has been canceled
    ERR_NOT_FOUND,          // A component is missing
    ERR_ALREADY_EXIST,      // A given unique-id is already taken
    ERR_ACCESS_DENIED,      // The operation couldn't be completed because it's missing privileges
    ERR_TARGET_BUSY,        // The target of the operation is busy
    ERR_NO_MORE_ID,         // There is no more ID available

    /* Permission errrors */
    ERR_PERMISSION_DENIED,  // The operation isn't allowed

    /* Memory-related errors */
    ERR_ALREADY_MAPPED,     // The given virtual address is already mapped
    ERR_NOT_MAPPED,         // The given virtual address is not mapped
    ERR_BAD_MEMORY,         // The given virtual buffer is unmapped or currupted
};

typedef enum status status_t;

static char const * const status_str[] = {
    [OK] = "OK",

    [ERR_INTERNAL]              = "ERR_INTERNAL",
    [ERR_NOT_IMPLEMENTED]       = "ERR_NOT_IMPLEMENTED",
    [ERR_NOT_SUPPORTED]         = "ERR_NOT_SUPPORTED",
    [ERR_OUT_OF_MEMORY]         = "ERR_OUT_OF_MEMORY",
    [ERR_INTERRUPTED]           = "ERR_INTERRUPTED",

    [ERR_INVALID_ARGS]          = "ERR_INVALID_ARGS",
    [ERR_OUT_OF_RANGE]          = "ERR_OUT_OF_RANGE",
    [ERR_BAD_HANDLER]           = "ERR_BAD_HANDLER",

    [ERR_BAD_STATE]             = "ERR_BAD_STATE",
    [ERR_TIMED_OUT]             = "ERR_TIMED_OUT",
    [ERR_CANCELED]              = "ERR_CANCELED",
    [ERR_NOT_FOUND]             = "ERR_NOT_FOUND",
    [ERR_ALREADY_EXIST]         = "ERR_ALREADY_EXIST",
    [ERR_ACCESS_DENIED]         = "ERR_ACCESS_DENIED",
    [ERR_TARGET_BUSY]           = "ERR_TARGET_BUSY",
    [ERR_NO_MORE_ID]            = "ERR_NO_MORE_ID",

    [ERR_PERMISSION_DENIED]     = "ERR_PERMISSION_DENIED",

    [ERR_ALREADY_MAPPED]        = "ERR_ALREADY_MAPPED",
    [ERR_NOT_MAPPED]            = "ERR_NOT_MAPPED",
    [ERR_BAD_MEMORY]            = "ERR_BAD_MEMORY",
};
