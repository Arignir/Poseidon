/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2022 - The Poseidon Authors
**
\******************************************************************************/

#include <poseidon/poseidon.h>
#include <arch/x86_64/cpuid.h>
#include <lib/string.h>
#include <lib/log.h>

static char const * const features_name[ARRAY_LENGTH(((struct cpuid *)NULL)->features.raw)][32] = {
    // EDX when CPUID.EAX=0x1
    [0] = {
        [0] = "fpu",
        [1] = "vme",
        [2] = "de",
        [3] = "pse",
        [4] = "tsc",
        [5] = "msr",
        [6] = "pae",
        [7] = "mce",
        [8] = "cx8",
        [9] = "apic",
        [11] = "sep",
        [12] = "mtrr",
        [13] = "pge",
        [14] = "mca",
        [15] = "cmov",
        [16] = "pat",
        [17] = "pse36",
        [18] = "psn",
        [19] = "clflush",
        [21] = "ds",
        [22] = "acpi",
        [23] = "mmx",
        [24] = "fxsr",
        [25] = "sse",
        [26] = "sse2",
        [27] = "ss",
        [28] = "htt",
        [29] = "tm",
        [31] = "bpe",
    },
    // ECX when CPUID.EAX=0x1
    [1] = {
        [0] = "sse3",
        [1] = "pclmulqdq",
        [2] = "dtes64",
        [3] = "monitor",
        [4] = "ds_cpl",
        [5] = "vmx",
        [6] = "smx",
        [7] = "eist",
        [8] = "tm2",
        [9] = "ssse3",
        [10] = "cnxt_id",
        [11] = "sdbg",
        [12] = "fma",
        [13] = "cmpxchg16b",
        [14] = "xtpr",
        [15] = "pdcm",
        [17] = "pcid",
        [18] = "dca",
        [19] = "sse4_1",
        [20] = "sse4_2",
        [21] = "x2apic",
        [22] = "movbe",
        [23] = "popcnt",
        [24] = "tsc_deadline",
        [25] = "aes",
        [26] = "xsave",
        [27] = "osxsave",
        [28] = "avx",
        [29] = "f16c",
        [30] = "rdrand",
    },
    // EBX when CPUID.EAX=0x7 and ECX=0
    [2] = {
        [0] = "fsgsbase",
        [1] = "tsc_adjust",
        [2] = "sgx",
        [3] = "bmi1",
        [4] = "hle",
        [5] = "avx2",
        [6] = "fdp_excptn_only",
        [7] = "smep",
        [8] = "bmi2",
        [9] = "erms",
        [10] = "invpcid",
        [11] = "rtm",
        [12] = "rdt_m",
        [13] = "fpu_cs_ds_depr",
        [14] = "mpx",
        [15] = "rdt_a",
        [18] = "rdseed",
        [19] = "adx",
        [20] = "smap",
        [23] = "clflushopt",
        [24] = "clwb",
        [25] = "intel_pt",
        [29] = "sha",
    },
    // ECX when CPUID.EAX=0x7 and ECX=0
    [3] = {
        [0] = "prefetchwt1",
        [2] = "umip",
        [3] = "pku",
        [4] = "ospke",
        [22] = "rdpid",
        [30] = "sgx_lc",
    },
    // ECX when CPUID.EAX=0x80000001
    [4] = {
        [0] = "lahf_lm",
        [5] = "lzcnt",
        [8] = "3dnowprefetch",
    },
    // EDX when CPUID.EAX=0x80000001
    [5] = {
        [11] = "syscall",
        [20] = "nx",
        [26] = "pdpe1gb",
        [27] = "rdtscp",
        [29] = "lm",
    },
};

/*
** Use the `cpuid` instructio to discover the features available to the currently
** running CPU.
**
** Results are stored in `features`.
*/
void
cpuid_load(
    struct cpuid *cpuid
) {
    memset(cpuid, 0, sizeof(*cpuid));

    uint32 *vendor_id = (uint32 *)cpuid->vendor_id;

    /*
    ** Load the vendor string, using the CPUID instruction with EAX=0,
    ** into EBX, EDX and ECX.
    */
    asm volatile(
        "cpuid"
        :
            "=a"(cpuid->max_cpuid),
            "=b"(*vendor_id),
            "=d"(*(vendor_id + 1)),
            "=c"(*(vendor_id + 2))
        : "a"(0x0)
        :
    );

    /*
    ** Load CPUID.EAX=0x1
    ** This returns:
    **   * Model, familiy and stepping information in EAX
    **   * Miscellaneous information in EBX
    **   * A list of some features the CPU support in ECX
    **   * A list of some features the CPU support in EDX
    */
    if (cpuid->max_cpuid >= 0x1) {
        uint32 ebx;

        asm volatile(
            "cpuid"
            :
                "=a"(cpuid->version.raw),
                "=b"(ebx),
                "=c"(cpuid->features.value_1_ecx),
                "=d"(cpuid->features.value_1_edx)
            : "a"(0x1)
            :
        );

        /*
        ** Calculate the display model and family ID, according to the Intel
        ** specification.
        */
        cpuid->display_family = cpuid->version.family_id;
        if (cpuid->version.family_id == 0xF) {
            cpuid->display_family += cpuid->version.extended_family_id;
        }

        cpuid->display_model = cpuid->version.model_id;
        if (cpuid->version.family_id == 0x6 || cpuid->version.family_id == 0xF) {
            cpuid->display_model += (cpuid->version.extended_model_id << 4);
        }

        /*
        ** Move the content of EBX into some fields of cpuid.
        **
        ** We did not use the common struct/union pattern here because they
        ** cannot really be unified under a common name.
        */

        cpuid->brand_idx = ebx & 0xF;
        cpuid->clflush_size = ((ebx >> 8) & 0xF) * 8;
        cpuid->max_logical_cpu = (ebx >> 16) & 0xF;
        cpuid->initial_apic_id = (ebx >> 24) & 0xF;
    }

    /*
    ** Load CPUID.EAX=0x7, ECX=0
    ** This returns:
    **   * A list of some features the CPU support in EBX
    **   * A list of some features the CPU support in ECX
    */
    if (cpuid->max_cpuid >= 0x7) {
        asm volatile(
            "cpuid"
            :
                "=b"(cpuid->features.value_7_0_ebx),
                "=c"(cpuid->features.value_7_0_ecx)
            : "a"(0x7), "c"(0x0)
            : "edx"
        );
    }

    // Load the maximum input value for extended function CPUID information.
    asm volatile(
        "cpuid"
        : "=a"(cpuid->max_extended_cpuid)
        : "a"(0x80000000)
        : "ebx", "ecx", "edx"
    );

    /*
    ** Load CPUID.EAX=0x80000001,
    ** This returns:
    **   * An extended list of some features the CPU support in ECX
    **   * An extended list of some features the CPU support in EDX
    */
    if (cpuid->max_extended_cpuid >= 0x80000001) {
        asm volatile(
            "cpuid"
            :
                "=c"(cpuid->features.value_8xx1_ecx),
                "=d"(cpuid->features.value_8xx1_edx)
            : "a"(0x80000001)
            : "ebx"
        );
    }

    // Load the brand information string, using multiple CPUID calls.
    if (cpuid->max_extended_cpuid >= 0x80000004) {
        uint32 *brand = (uint32 *)cpuid->brand;

        for (uint i = 0; i < 3; ++i) {
            asm volatile(
                "cpuid"
                :
                    "=a"(*(brand + i * 4 + 0)),
                    "=b"(*(brand + i * 4 + 1)),
                    "=c"(*(brand + i * 4 + 2)),
                    "=d"(*(brand + i * 4 + 3))
                : "a"(0x80000002 + i)
                :
            );
        }
    }

    // Find the maximum physical address number
    if (cpuid->max_extended_cpuid >= 0x80000008) {
        uint8 eax[4];
        asm volatile(
            "cpuid"
            :
                "=a"(*(uint32 *)eax)
            : "a"(0x80000008)
            : "ebx", "ecx", "edx"
        );
        cpuid->maxphyaddr = eax[0];
        cpuid->maxvirtaddr = eax[1];
    } else {
        /*
        ** The intel spec gives default values if CPUID function 0x80000008
        ** isn't supported.
        */
        if (cpuid->features.pae) {
            cpuid->maxphyaddr = 48;
        } else {
            cpuid->maxphyaddr = 32;
        }
        cpuid->maxvirtaddr = 32;
    }
}

/*
** Dump the findings of the `cpuid_load()` to the console in a
** user-readable fashion.
*/
void
cpuid_dump(
    struct cpuid *cpuid
) {
    logln("vendor_id        | %s", cpuid->vendor_id);
    logln("family           | %i", cpuid->display_family);
    logln("model            | %i", cpuid->display_model);
    logln("model name       | %s", cpuid->brand);
    logln("stepping         | %i", cpuid->version.stepping_id);
    logln("clflush size     | %i", cpuid->clflush_size);
    logln(
        "address size     | %i bits physical, %i bits virtual",
        cpuid->maxphyaddr,
        cpuid->maxvirtaddr
    );

    log("flag             |");
    for (uint i = 0; i < ARRAY_LENGTH(cpuid->features.raw); ++i) {
        for (uint j = 0; j < 32; ++j) {
            char const *feature_name = features_name[i][j];
            if (feature_name) { // feature_name is NULL for reserved bytes
                if ((cpuid->features.raw[i] >> j & 0x1)) {
                    log(" %s", feature_name);
                }
            }
        }
    }
    logln("");
}
