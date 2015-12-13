//
// Dumps the CP15 registers for an ARM11 MPCore
// which is used in the 3DS
//
// If you try to adapt this to another system, make sure you change the MRC ops accordingly.
//

#include <cstdio>

void DumpCP15Registers(bool print_system_mode_regs)
{
    unsigned int c0[16] = {};
    unsigned int c1[3] = {};
    unsigned int c2[3] = {};
    unsigned int c3 = 0;
    unsigned int c5[2] = {};
    unsigned int c6[2] = {};
    unsigned int paddr_reg = 0;
    unsigned int c10[3] = {};
    unsigned int c13[5] = {};
    unsigned int c15[9] = {};

    if (print_system_mode_regs) {
        asm volatile("MRC p15, 0, %[out], c0, c0, 0" : [out]"=&r"(c0[0])  ); // Main ID
        asm volatile("MRC p15, 0, %[out], c0, c0, 1" : [out]"=&r"(c0[1])  ); // Cache Type
        asm volatile("MRC p15, 0, %[out], c0, c0, 3" : [out]"=&r"(c0[2])  ); // TLB Type
        asm volatile("MRC p15, 0, %[out], c0, c0, 5" : [out]"=&r"(c0[3])  ); // CPU ID
        asm volatile("MRC p15, 0, %[out], c0, c1, 0" : [out]"=&r"(c0[4])  ); // Proc feature register 0
        asm volatile("MRC p15, 0, %[out], c0, c1, 1" : [out]"=&r"(c0[5])  ); // Proc feature register 1
        asm volatile("MRC p15, 0, %[out], c0, c1, 2" : [out]"=&r"(c0[6])  ); // Debug feature register
        asm volatile("MRC p15, 0, %[out], c0, c1, 4" : [out]"=&r"(c0[7])  ); // Memory feature register 0
        asm volatile("MRC p15, 0, %[out], c0, c1, 5" : [out]"=&r"(c0[8])  ); // Memory feature register 1
        asm volatile("MRC p15, 0, %[out], c0, c1, 6" : [out]"=&r"(c0[9])  ); // Memory feature register 2
        asm volatile("MRC p15, 0, %[out], c0, c1, 7" : [out]"=&r"(c0[10]) ); // Memory feature register 3
        asm volatile("MRC p15, 0, %[out], c0, c2, 0" : [out]"=&r"(c0[11]) ); // Instruction set attribute register 0
        asm volatile("MRC p15, 0, %[out], c0, c2, 1" : [out]"=&r"(c0[12]) ); // Instruction set attribute register 1
        asm volatile("MRC p15, 0, %[out], c0, c2, 2" : [out]"=&r"(c0[13]) ); // Instruction set attribute register 2
        asm volatile("MRC p15, 0, %[out], c0, c2, 3" : [out]"=&r"(c0[14]) ); // Instruction set attribute register 3
        asm volatile("MRC p15, 0, %[out], c0, c2, 4" : [out]"=&r"(c0[15]) ); // Instruction set attribute register 4

        // NOTE: These can be read/written, so these aren't constant values.
        asm volatile("MRC p15, 0, %[out], c1, c0, 0" : [out]"=&r"(c1[0])  ); // Control register
        asm volatile("MRC p15, 0, %[out], c1, c0, 1" : [out]"=&r"(c1[1])  ); // Auxiliary control register
        asm volatile("MRC p15, 0, %[out], c1, c0, 2" : [out]"=&r"(c1[2])  ); // Coprocessor access control register

        // NOTE: These can be read/written, so these aren't constant values.
        asm volatile("MRC p15, 0, %[out], c2, c0, 0" : [out]"=&r"(c2[0])  ); // Translation table base register 0
        asm volatile("MRC p15, 0, %[out], c2, c0, 1" : [out]"=&r"(c2[1])  ); // Translation table base register 1
        asm volatile("MRC p15, 0, %[out], c2, c0, 2" : [out]"=&r"(c2[2])  ); // Translation table base control

        // NOTE: This can be read/written, so these aren't constant values.
        asm volatile("MRC p15, 0, %[out], c3, c0, 0" : [out]"=&r"(c3)     ); // Domain access control register

        // NOTE: These can be read/written, so these aren't constant values.
        asm volatile("MRC p15, 0, %[out], c5, c0, 0" : [out]"=&r"(c5[0])  ); // Data fault status register
        asm volatile("MRC p15, 0, %[out], c5, c0, 1" : [out]"=&r"(c5[1])  ); // Instruction fault status register

        // NOTE: These can be read/written, so these aren't constant values.
        asm volatile("MRC p15, 0, %[out], c6, c0, 0" : [out]"=&r"(c6[0])  ); // Fault address register
        asm volatile("MRC p15, 0, %[out], c6, c0, 1" : [out]"=&r"(c6[1])  ); // Watchpoint fault address register

        // Only one readable register in c7. Note that this can also be written to.
        asm volatile("MRC p15, 0, %[out], c7, c4, 0" : [out]"=&r"(paddr_reg)); // Physical address register

        // NOTE: These can be read/written, so these aren't constant values.
        asm volatile("MRC p15, 0, %[out], c10, c0, 0" : [out]"=&r"(c10[0])); // TLB lockdown register
        asm volatile("MRC p15, 0, %[out], c10, c2, 0" : [out]"=&r"(c10[1])); // Primary remap register
        asm volatile("MRC p15, 0, %[out], c10, c2, 1" : [out]"=&r"(c10[2])); // Normal remap register

        // c13
        // NOTE: These can be read/written, so these aren't constant values.
        asm volatile("MRC p15, 0, %[out], c13, c0, 0" : [out]"=&r"(c13[0])); // Process ID register
        asm volatile("MRC p15, 0, %[out], c13, c0, 1" : [out]"=&r"(c13[1])); // Context ID register
    }
    asm volatile("MRC p15, 0, %[out], c13, c0, 2" : [out]"=&r"(c13[2])    ); // Thread ID register 0 (User read/write | System read/write)
    asm volatile("MRC p15, 0, %[out], c13, c0, 3" : [out]"=&r"(c13[3])    ); // Thread ID register 1 (User read | System read/write)
    if (print_system_mode_regs) {
        asm volatile("MRC p15, 0, %[out], c13, c0, 4" : [out]"=&r"(c13[4])); // Thread ID register 2 (System read/write)

        // c15
        // NOTE: These can be read/written, so these aren't constant values.
        asm volatile("MRC p15, 0, %[out], c15, c12, 0" : [out]"=&r"(c15[0]) ); // Performance Monitor Control Register
        asm volatile("MRC p15, 0, %[out], c15, c12, 1" : [out]"=&r"(c15[1]) ); // Cycle counter register
        asm volatile("MRC p15, 0, %[out], c15, c12, 2" : [out]"=&r"(c15[2]) ); // Count Register 0
        asm volatile("MRC p15, 0, %[out], c15, c12, 3" : [out]"=&r"(c15[3]) ); // Count Register 1
        asm volatile("MRC p15, 5, %[out], c15, c4, 2" : [out]"=&r"(c15[4])  ); // Main TLB entry register
        asm volatile("MRC p15, 5, %[out], c15, c5, 2" : [out]"=&r"(c15[5])  ); // Main TLB VA register
        asm volatile("MRC p15, 5, %[out], c15, c6, 2" : [out]"=&r"(c15[6])  ); // Main TLB PA register
        asm volatile("MRC p15, 5, %[out], c15, c7, 2" : [out]"=&r"(c15[7])  ); // Main TLB attribute register
        asm volatile("MRC p15, 7, %[out], c15, c1, 0" : [out]"=&r"(c15[7])  ); // TLB debug control register

        printf("-- c0 Registers --\n");
        printf("Main ID    : 0x%08X\n", c0[0]);
        printf("Cache Type : 0x%08X\n", c0[1]);
        printf("TLB Type   : 0x%08X\n", c0[2]);
        printf("CPU ID     : 0x%08X\n", c0[3]);
        printf("Processor Feature Register 0   : 0x%08X\n", c0[4]);
        printf("Processor Feature Register 1   : 0x%08X\n", c0[5]);
        printf("Debug Feature Register 0       : 0x%08X\n", c0[6]);
        printf("Memory Feature Register 0      : 0x%08X\n", c0[7]);
        printf("Memory Feature Register 1      : 0x%08X\n", c0[8]);
        printf("Memory Feature Register 2      : 0x%08X\n", c0[9]);
        printf("Memory Feature Register 3      : 0x%08X\n", c0[10]);
        printf("Instruction Feature Register 0 : 0x%08X\n", c0[11]);
        printf("Instruction Feature Register 1 : 0x%08X\n", c0[12]);
        printf("Instruction Feature Register 2 : 0x%08X\n", c0[13]);
        printf("Instruction Feature Register 3 : 0x%08X\n", c0[14]);
        printf("Instruction Feature Register 4 : 0x%08X\n", c0[15]);

        printf("-- c1 Registers --\n");
        printf("Control Register                    : 0x%08X\n", c1[0]);
        printf("Auxiliary Control Register          : 0x%08X\n", c1[1]);
        printf("Coprocessor Access Control Register : 0x%08X\n", c1[2]);

        printf("-- c2 Registers --\n");
        printf("Translation Table Base Register 0 : 0x%08X\n", c2[0]);
        printf("Translation Table Base Register 1 : 0x%08X\n", c2[1]);
        printf("Translation Table Base Control    : 0x%08X\n", c2[2]);

        printf("-- c3 Registers --\n");
        printf("Domain Access Control Register : 0x%08X\n", c3);

        printf("-- c5 Registers --\n");
        printf("Data Fault Status Register        : 0x%08X\n", c5[0]);
        printf("Instruction Fault Status Register : 0x%08X\n", c5[1]);

        printf("-- c6 Registers --\n");
        printf("Fault Address Register            : 0x%08X\n", c6[0]);
        printf("Watchpoint Fault Address Register : 0x%08X\n", c6[1]);

        printf("-- c7 Registers --\n");
        printf("Physical Address Register : 0x%08X\n", paddr_reg);

        printf("-- c10 Registers --\n");
        printf("TLB Lockdown Register  : 0%08X\n", c10[0]);
        printf("Primary Remap Register : 0%08X\n", c10[1]);
        printf("Normal Remap Register  : 0%08X\n", c10[2]);
    }

    printf("-- c13 Registers --\n");
    if (print_system_mode_regs) {
        printf("Processor ID Register : 0x%08X\n", c13[0]);
        printf("Context ID Register   : 0x%08X\n", c13[1]);
    }
    printf("Thread ID Register (User R/W | Sys R/W) : 0x%08X\n", c13[2]);
    printf("Thread ID Register (User R | Sys R/W)   : 0x%08X\n", c13[3]);
    if (print_system_mode_regs) {
        printf("Thread ID Register (Sys R/W)            : 0x%08X\n", c13[4]);

        printf("-- c15 Registers --\n");
        printf("Performance Monitor Control Register : 0x%08X\n", c15[0]);
        printf("Cycle Counter Register      : 0x%08X\n", c15[1]);
        printf("Count Register 0            : 0x%08X\n", c15[2]);
        printf("Count Register 1            : 0x%08X\n", c15[3]);
        printf("Main TLB Entry Register     : 0x%08X\n", c15[4]);
        printf("Main TLB VA Register        : 0x%08X\n", c15[5]);
        printf("Main TLB PA Register        : 0x%08X\n", c15[6]);
        printf("Main TLB Attribute Register : 0x%08X\n", c15[7]);
        printf("TLB Debug Control Register  : 0x%08X\n", c15[8]);
    }
}