/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

#ifndef _ARCH_X86_CPUID_H_
# define _ARCH_X86_CPUID_H_

# include <poseidon/poseidon.h>

/*
** The output of CPUID.
**
** This structure is not exhaustive, feel free to complete it.
*/
struct cpuid {
    uint32 max_cpuid;                   // The max. input value for CPUID
    uint32 max_extended_cpuid;          // The max. input value for extended CPUID

    char vendor_id[13];                 // CPU Vendor ID String (null terminated)
    char brand[49];                     // CPU Brand String (null terminated)

    // Content of EAX (CPUID.EAX=0x1)
    union {
        struct {
            uint32 stepping_id: 4;
            uint32 model_id: 4;
            uint32 family_id: 4;
            uint32 processor_type: 2;
            uint32 _reserved_0: 2;
            uint32 extended_model_id: 4;
            uint32 extended_family_id: 8;
            uint32 _reserved_1: 4;
        };
        uint32 value;
    } version;

    // Content of EBX (CPUID.EAX=0x1)
    uint8 brand_idx;
    uint32 clflush_size;                // Cash line size, in bytes
    uint8 max_logical_cpu;
    uint8 initial_apic_id;              // Not updated when the APIC ID is modified

    /*
    ** The two following values are obtained by combining the {model,family}_id
    ** and their extended counterpart.
    */

    uint32 display_family;              // Beautified family ID
    uint32 display_model;               // Beautified model ID

    // Feature flags
    union {
        struct  {
            // EDX when CPUID.EAX=0x1
            uint32 fpu: 1;              // x87-FPU on Chip
            uint32 vme: 1;              // Virtual-8086 Mode Enhancement
            uint32 de: 1;               // Debugging Extensions
            uint32 pse: 1;              // Page Size Extensions
            uint32 tsc: 1;              // Time Stamp Counter
            uint32 msr: 1;              // RDMSR and WRMSR Support
            uint32 pae: 1;              // Physical Address Extension
            uint32 mce: 1;              // Machine-Check extension
            uint32 cx8: 1;              // CMPXCHG8B Instruction
            uint32 apic: 1;             // APIC on a Chip
            uint32 _reserved_0: 1;
            uint32 sep: 1;              // SYSENTER and SYSEXIT
            uint32 mtrr: 1;             // Memory Type Range Register
            uint32 pge: 1;              // PTE Global Bit
            uint32 mca: 1;              // Machine Check Architecture
            uint32 cmov: 1;             // Conditional Move/Compare Instruction
            uint32 pat: 1;              // Page Attribute Table
            uint32 pse36: 1;            // Page Size Extension
            uint32 psn: 1;              // Processor Serial Number
            uint32 clflush: 1;          // CLFLUSH Instruction
            uint32 _reserved_1: 1;
            uint32 ds: 1;               // Debug Store
            uint32 acpi: 1;             // Thermal Monitor and Clock Ctrl
            uint32 mmx: 1;              // MMX Technology
            uint32 fxsr: 1;             // FXSAVE/FXSTORE
            uint32 sse: 1;              // SSE extensions
            uint32 sse2: 1;             // SSE2 extensions
            uint32 ss: 1;               // Self Snoop
            uint32 htt: 1;              // Multi-threading
            uint32 tm: 1;               // Thermal monitor
            uint32 _reserved_2: 1;
            uint32 bpe: 1;              // Pend. Brk. EN.

            // ECX when CPUID.EAX=0x1
            uint32 sse3: 1;             // SSE3 Extensions
            uint32 pclmulqdq: 1;        // Carryless Multiplication
            uint32 dtes64: 1;           // 64-bit DS Area
            uint32 monitor: 1;          // MONITOR/MWAIT
            uint32 ds_cpl: 1;           // CPL Qualified Debug Store
            uint32 vmx: 1;              // Virtual Machine Extensions
            uint32 smx: 1;              // Safer Mode Extensions
            uint32 eist: 1;             // Enhanced Intel SpeedStep Technology
            uint32 tm2: 1;              // Thermal Monitor 2
            uint32 ssse3: 1;            // SSSE3 Extensions
            uint32 cnxt_id: 1;          // L1 Context ID
            uint32 sdbg: 1;             // Silicon Debug
            uint32 fma: 1;              // Fused Multiply Add
            uint32 cmpxchg16b: 1;       // "Compare and Exchange Bytes" Instructions
            uint32 xtpr: 1;             // xTPR Update Control
            uint32 pdcm: 1;             // Perf/Debug Capability MSR
            uint32 _reserved_3: 1;
            uint32 pcid: 1;             // Process-Context Identifiers
            uint32 dca: 1;              // Direct Cache Access
            uint32 sse4_1: 1;           // SSE4.1 Extensions
            uint32 sse4_2: 1;           // SSE4.2 Extensions
            uint32 x2apic: 1;           // x2APIC
            uint32 movbe: 1;            // MOVBE Instructions
            uint32 popcnt: 1;           // POPCNT Instructions
            uint32 tsc_deadline: 1;     // APIC Timer supports TSC deadline
            uint32 aes: 1;              // AES Instructions
            uint32 xsave: 1;            // Support for XSAVE/XRSTOR extended states, XSETBV/XGETVB, XCR0
            uint32 osxsave: 1;          // xave is enabled by the OS
            uint32 avx: 1;              // AVX Instructions
            uint32 f16c: 1;             // 16-bit Floating-Point Conversions Instructions
            uint32 rdrand: 1;           // RDRAND Instructions
            uint32 _reserved_4: 1;

            // EBX when CPUID.EAX=0x7 and ECX=0
            uint32 fsgsbase: 1;         // Support for {RD,WR}{FS,GS}BASE
            uint32 tsc_adjust: 1;       // IA32_TSC_ADJUST MSR
            uint32 sgx: 1;              // Software Guard Extensions
            uint32 bmi1: 1;             // Bit Manipulation Instruction Sets
            uint32 hle: 1;              // Hardware Lock Elision
            uint32 avx2: 1;             // Advanced Vector Extension 2
            uint32 fdp_excptn_only: 1;  // Updated x87 fpu data pointer only on x87 exceptions
            uint32 smep: 1;             // Supervisor-Mode Execution Prevention
            uint32 bmi2: 1;             // Bit Manipulation Instruction Sets 2
            uint32 erms: 1;             // Enhanced REP MOVSB/STOSB
            uint32 invpcid: 1;          // INVPCID Instruction
            uint32 rtm: 1;              // Restricted Transactional Memory
            uint32 rdt_m: 1;            // Resource Director Technology Monitoring capability
            uint32 fpu_cs_ds_depr: 1;   // Deprecates FPU CS and DS values
            uint32 mpx: 1;              // Memory Protection Extensions
            uint32 rdt_a: 1;            // Resource Director Technology Allocation capability
            uint32 _reserved_5: 2;
            uint32 rdseed: 1;           // RDSEED Instruction
            uint32 adx: 1;              // Arbitrary-Precision Arithmetic Extension
            uint32 smap: 1;             // Supervisor-Mode Access Prevention
            uint32 _reserved_6: 2;
            uint32 clflushopt: 1;       // CLFLUSHOPT Instruction
            uint32 clwb: 1;             // Cache Line Write Back
            uint32 intel_pt: 1;         // Intel Processor Trace
            uint32 _reserved_7: 3;
            uint32 sha: 1;              // Secure Hash Algorithm Extensions
            uint32 _reserved_8: 2;

            // ECX when CPUID.EAX=0x7 and ECX=0
            uint32 prefetchwt1: 1;      // PREFETCHWT1 Instruction
            uint32 _reserved_9: 1;
            uint32 umip: 1;             // User-mode Instruction Prevention
            uint32 pku: 1;              // Protection Keys for User-Mode Pages
            uint32 ospke: 1;            // Protection keys are enabled by the OS (CR4.PKE)
            uint32 _reserved_10: 12;
            uint32 mawau: 5;            // Value of MAWAU used by the BNDLDX and BNDSTX instructions
            uint32 rdpid: 1;            // Supports Read Processor ID
            uint32 _reserved_11: 7;
            uint32 sgx_lc: 1;           // SGX Launch Configuration
            uint32 _reserved_12: 1;
        };
        struct {
            uint32 value_1_edx;     // The value of EDX when CPUID.EAX=0x1
            uint32 value_1_ecx;     // The value of ECX when CPUID.EAX=0x1
            uint32 value_7_0_ebx;   // The value of EBX when CPUID.EAX=0x7 and ECX=0
            uint32 value_7_0_ecx;   // The value of ECX when CPUID.EAX=0x7 and ECX=0
        };
        uint32 values[4];
    } features;
};

static_assert(sizeof(((struct cpuid *)NULL)->version) == sizeof(uint32));
static_assert(sizeof(((struct cpuid *)NULL)->features) == 4 * sizeof(uint32));

extern struct cpuid cpu_features;

bool        detect_cpuid(void);
void        load_cpuid_features(void);
void        dump_cpuid(void);

#endif /* !_ARCH_X86_CPUID_H_ */
