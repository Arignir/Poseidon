/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _ARCH_X86_64_CPUID_H_
# define _ARCH_X86_64_CPUID_H_

# include <poseidon/poseidon.h>

/*
** The output of CPUID.
**
** This structure is not exhaustive, feel free to complete it.
*/
struct cpuid {
    uint32 max_cpuid;                   // The max. input value for CPUID
    uint32 max_extended_cpuid;          // The max. input value for extended CPUID

    // The maximum physical-address width supported by the processor, *in bits*.
    // This is equivalent to the constant MAXPHYADDR used throughouh the intel
    // spec.
    uint maxphyaddr;

    // The maximum virtual-address width supported by the processor, *in bits*.
    uint maxvirtaddr;

    char vendor_id[13];                 // CPU Vendor ID String (null terminated)
    char brand[49];                     // CPU Brand String (null terminated)

    // Content of EAX (CPUID.EAX=0x1)
    union {
        struct {
            size_t stepping_id: 4;
            size_t model_id: 4;
            size_t family_id: 4;
            size_t processor_type: 2;
            size_t : 2;
            size_t extended_model_id: 4;
            size_t extended_family_id: 8;
            size_t : 4;
        } __packed;
        uint32 raw;
    } __packed version;

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
        struct {
            // EDX when CPUID.EAX=0x1
            size_t fpu: 1;              // x87-FPU on Chip
            size_t vme: 1;              // Virtual-8086 Mode Enhancement
            size_t de: 1;               // Debugging Extensions
            size_t pse: 1;              // Page Size Extensions
            size_t tsc: 1;              // Time Stamp Counter
            size_t msr: 1;              // RDMSR and WRMSR Support
            size_t pae: 1;              // Physical Address Extension
            size_t mce: 1;              // Machine-Check extension
            size_t cx8: 1;              // CMPXCHG8B Instruction
            size_t apic: 1;             // APIC on a Chip
            size_t : 1;
            size_t sep: 1;              // SYSENTER and SYSEXIT
            size_t mtrr: 1;             // Memory Type Range Register
            size_t pge: 1;              // PTE Global Bit
            size_t mca: 1;              // Machine Check Architecture
            size_t cmov: 1;             // Conditional Move/Compare Instruction
            size_t pat: 1;              // Page Attribute Table
            size_t pse36: 1;            // Page Size Extension
            size_t psn: 1;              // Processor Serial Number
            size_t clflush: 1;          // CLFLUSH Instruction
            size_t : 1;
            size_t ds: 1;               // Debug Store
            size_t acpi: 1;             // Thermal Monitor and Clock Ctrl
            size_t mmx: 1;              // MMX Technology
            size_t fxsr: 1;             // FXSAVE/FXSTORE
            size_t sse: 1;              // SSE extensions
            size_t sse2: 1;             // SSE2 extensions
            size_t ss: 1;               // Self Snoop
            size_t htt: 1;              // Multi-threading
            size_t tm: 1;               // Thermal monitor
            size_t : 1;
            size_t bpe: 1;              // Pend. Brk. EN.

            // ECX when CPUID.EAX=0x1
            size_t sse3: 1;             // SSE3 Extensions
            size_t pclmulqdq: 1;        // Carryless Multiplication
            size_t dtes64: 1;           // 64-bit DS Area
            size_t monitor: 1;          // MONITOR/MWAIT
            size_t ds_cpl: 1;           // CPL Qualified Debug Store
            size_t vmx: 1;              // Virtual Machine Extensions
            size_t smx: 1;              // Safer Mode Extensions
            size_t eist: 1;             // Enhanced Intel SpeedStep Technology
            size_t tm2: 1;              // Thermal Monitor 2
            size_t ssse3: 1;            // SSSE3 Extensions
            size_t cnxt_id: 1;          // L1 Context ID
            size_t sdbg: 1;             // Silicon Debug
            size_t fma: 1;              // Fused Multiply Add
            size_t cmpxchg16b: 1;       // "Compare and Exchange Bytes" Instructions
            size_t xtpr: 1;             // xTPR Update Control
            size_t pdcm: 1;             // Perf/Debug Capability MSR
            size_t : 1;
            size_t pcid: 1;             // Process-Context Identifiers
            size_t dca: 1;              // Direct Cache Access
            size_t sse4_1: 1;           // SSE4.1 Extensions
            size_t sse4_2: 1;           // SSE4.2 Extensions
            size_t x2apic: 1;           // x2APIC
            size_t movbe: 1;            // MOVBE Instructions
            size_t popcnt: 1;           // POPCNT Instructions
            size_t tsc_deadline: 1;     // APIC Timer supports TSC deadline
            size_t aes: 1;              // AES Instructions
            size_t xsave: 1;            // Support for XSAVE/XRSTOR extended states, XSETBV/XGETVB, XCR0
            size_t osxsave: 1;          // xave is enabled by the OS
            size_t avx: 1;              // AVX Instructions
            size_t f16c: 1;             // 16-bit Floating-Point Conversions Instructions
            size_t rdrand: 1;           // RDRAND Instructions
            size_t : 1;

            // EBX when CPUID.EAX=0x7 and ECX=0
            size_t fsgsbase: 1;         // Support for {RD,WR}{FS,GS}BASE
            size_t tsc_adjust: 1;       // IA32_TSC_ADJUST MSR
            size_t sgx: 1;              // Software Guard Extensions
            size_t bmi1: 1;             // Bit Manipulation Instruction Sets
            size_t hle: 1;              // Hardware Lock Elision
            size_t avx2: 1;             // Advanced Vector Extension 2
            size_t fdp_excptn_only: 1;  // Updated x87 fpu data pointer only on x87 exceptions
            size_t smep: 1;             // Supervisor-Mode Execution Prevention
            size_t bmi2: 1;             // Bit Manipulation Instruction Sets 2
            size_t erms: 1;             // Enhanced REP MOVSB/STOSB
            size_t invpcid: 1;          // INVPCID Instruction
            size_t rtm: 1;              // Restricted Transactional Memory
            size_t rdt_m: 1;            // Resource Director Technology Monitoring capability
            size_t fpu_cs_ds_depr: 1;   // Deprecates FPU CS and DS values
            size_t mpx: 1;              // Memory Protection Extensions
            size_t rdt_a: 1;            // Resource Director Technology Allocation capability
            size_t : 2;
            size_t rdseed: 1;           // RDSEED Instruction
            size_t adx: 1;              // Arbitrary-Precision Arithmetic Extension
            size_t smap: 1;             // Supervisor-Mode Access Prevention
            size_t : 2;
            size_t clflushopt: 1;       // CLFLUSHOPT Instruction
            size_t clwb: 1;             // Cache Line Write Back
            size_t intel_pt: 1;         // Intel Processor Trace
            size_t : 3;
            size_t sha: 1;              // Secure Hash Algorithm Extensions
            size_t : 2;

            // ECX when CPUID.EAX=0x7 and ECX=0
            size_t prefetchwt1: 1;      // PREFETCHWT1 Instruction
            size_t : 1;
            size_t umip: 1;             // User-mode Instruction Prevention
            size_t pku: 1;              // Protection Keys for User-Mode Pages
            size_t ospke: 1;            // Protection keys are enabled by the OS (CR4.PKE)
            size_t : 12;
            size_t mawau: 5;            // Value of MAWAU used by the BNDLDX and BNDSTX instructions
            size_t rdpid: 1;            // Supports Read Processor ID
            size_t : 7;
            size_t sgx_lc: 1;           // SGX Launch Configuration
            size_t : 1;

            // ECX when CPUID.EAX=0x80000001
            size_t lahf_lm: 1;          // LAHF/SAHF available in 64-bit mode.
            size_t : 4;
            size_t lzcnt: 1;            // LZCNT instruction (count the number of leading zero bits)
            size_t : 2;
            size_t prefetchw: 1;        // Prefetchw instruction
            size_t : 23;

            // EDX when CPUID.EAX=0x80000001
            size_t : 11;
            size_t syscall: 1;          // SYSCALL/SYSRET instructions
            size_t : 8;
            size_t nx: 1;               // Execute Disable Bit Available
            size_t : 5;
            size_t pdpe1gb: 1;          // 1GB pages
            size_t rdtscp: 1;           // RDTSCP and IA32_TSC_AUX
            size_t : 1;
            size_t lm: 1;               // Long mode is available
            size_t : 2;
        } __packed;

        struct {
            uint32 value_1_edx;     // The value of EDX when CPUID.EAX=0x1
            uint32 value_1_ecx;     // The value of ECX when CPUID.EAX=0x1
            uint32 value_7_0_ebx;   // The value of EBX when CPUID.EAX=0x7 and ECX=0
            uint32 value_7_0_ecx;   // The value of ECX when CPUID.EAX=0x7 and ECX=0

            uint32 value_8xx1_ecx;  // The value of ECX when CPUID.EAX=0x80000001
            uint32 value_8xx1_edx;  // The value of EDX when CPUID.EAX=0x80000001
        } __packed;
        uint32 raw[6];
    } __packed features;
};

static_assert(sizeof(((struct cpuid *)NULL)->version) == sizeof(uint32));
static_assert(sizeof(((struct cpuid *)NULL)->features) == 6 * sizeof(uint32));

void cpuid_load(struct cpuid *);
void cpuid_dump(struct cpuid *);

#endif /* !_ARCH_X86_64_CPUID_H_ */
