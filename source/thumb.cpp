#include <cstdio>

static int gen_cvin(int cvin)
{
    int r = ((cvin & 2) ? (1<<29) : 0) | ((cvin & 1) ? (1<<28) : 0);
    r |= (1 << 31) | (1 << 30);
    return r;
}

// test macros to generate and output the result of a single instruction

// 1 registers in the insn, zero args: rD = op()
#define TESTINST1(instruction, RD, cvin) \
{ \
    unsigned int out; \
    unsigned int cpsr; \
\
    asm volatile( \
        "msr cpsr_f, %2;" \
        instruction ";" \
        "mov %0," #RD ";" \
        "mrs %1,cpsr;" \
        : "=&r" (out), "=&r" (cpsr) \
        : "r" (gen_cvin(cvin))      \
        : #RD, "cc", "memory" \
    ); \
    printf("%s :: rd 0x%08x, c:v-in %d, cpsr 0x%08x %c%c%c%c\n", \
        instruction, out, \
        cvin, \
        cpsr & 0xffff0000, \
        ((1<<31) & cpsr) ? 'N' : ' ', \
        ((1<<30) & cpsr) ? 'Z' : ' ', \
        ((1<<29) & cpsr) ? 'C' : ' ', \
        ((1<<28) & cpsr) ? 'V' : ' ' \
        ); \
}

// 1 registers in the insn, one args: rD = op(rD)
#define TESTINST1x(instruction, RDval, RD, cvin)       \
{ \
    unsigned int out; \
    unsigned int cpsr; \
\
    asm volatile( \
        "msr cpsr_f, %2;" \
        "mov " #RD ",%3;" \
        instruction ";" \
        "mov %0," #RD ";" \
        "mrs %1,cpsr;" \
        : "=&r" (out), "=&r" (cpsr) \
        : "r" (gen_cvin(cvin)), "r"(RDval) \
        : #RD, "cc", "memory" \
    ); \
    printf("%s :: rd 0x%08x, c:v-in %d, cpsr 0x%08x %c%c%c%c\n", \
        instruction, out, \
        cvin, \
        cpsr & 0xffff0000, \
        ((1<<31) & cpsr) ? 'N' : ' ', \
        ((1<<30) & cpsr) ? 'Z' : ' ', \
        ((1<<29) & cpsr) ? 'C' : ' ', \
        ((1<<28) & cpsr) ? 'V' : ' ' \
        ); \
}

// 2 registers in the insn, one arg: rD = op(rM)
#define TESTINST2(instruction, RMval, RD, RM, cvin) \
{ \
    unsigned int out; \
    unsigned int cpsr; \
\
    asm volatile( \
        "msr cpsr_f, %3;" \
        "mov " #RM ",%2;" \
        /* set #RD to 0x55555555 so we can see which parts get overwritten */ \
        "mov " #RD ", #0x55" "\n\t" \
        "orr " #RD "," #RD "," #RD ", LSL #8" "\n\t" \
        "orr " #RD "," #RD "," #RD ", LSL #16" "\n\t" \
        instruction ";" \
        "mov %0," #RD ";" \
        "mrs %1,cpsr;" \
        : "=&r" (out), "=&r" (cpsr) \
        : "r" (RMval), "r" (gen_cvin(cvin))  \
        : #RD, #RM, "cc", "memory" \
    ); \
    printf("%s :: rd 0x%08x rm 0x%08x, c:v-in %d, cpsr 0x%08x %c%c%c%c\n", \
        instruction, out, RMval, \
        cvin, \
        cpsr & 0xffff0000, \
        ((1<<31) & cpsr) ? 'N' : ' ', \
        ((1<<30) & cpsr) ? 'Z' : ' ', \
        ((1<<29) & cpsr) ? 'C' : ' ', \
        ((1<<28) & cpsr) ? 'V' : ' ' \
        ); \
}


// 2 registers in the insn, two args: rD = op(rD, rM)
#define TESTINST2x(instruction, RDval, RMval, RD, RM, cvin)       \
{ \
    unsigned int out; \
    unsigned int cpsr; \
\
    asm volatile( \
        "msr cpsr_f, %3;" \
        "mov " #RM ",%2;" \
        "mov " #RD ",%4;" \
        instruction ";" \
        "mov %0," #RD ";" \
        "mrs %1,cpsr;" \
        : "=&r" (out), "=&r" (cpsr) \
        : "r" (RMval), "r" (gen_cvin(cvin)), "r"(RDval) \
        : #RD, #RM, "cc", "memory" \
    ); \
    printf("%s :: rd 0x%08x rm 0x%08x, c:v-in %d, cpsr 0x%08x %c%c%c%c\n", \
        instruction, out, RMval, \
        cvin, \
        cpsr & 0xffff0000, \
        ((1<<31) & cpsr) ? 'N' : ' ', \
        ((1<<30) & cpsr) ? 'Z' : ' ', \
        ((1<<29) & cpsr) ? 'C' : ' ', \
        ((1<<28) & cpsr) ? 'V' : ' ' \
        ); \
}



#define TESTINST3(instruction, RMval, RNval, RD, RM, RN, cvin) \
{ \
    unsigned int out; \
    unsigned int cpsr; \
\
    asm volatile( \
        "msr cpsr_f, %4;" \
        "mov " #RM ",%2;" \
        "mov " #RN ",%3;" \
        instruction ";" \
        "mov %0," #RD ";" \
        "mrs %1,cpsr;" \
        : "=&r" (out), "=&r" (cpsr) \
        : "r" (RMval), "r" (RNval), "r" (gen_cvin(cvin))    \
        : #RD, #RM, #RN, "cc", "memory" \
    ); \
    printf("%s :: rd 0x%08x rm 0x%08x, rn 0x%08x, c:v-in %d, cpsr 0x%08x %c%c%c%c\n", \
        instruction, out, RMval, RNval, \
        cvin, \
        cpsr & 0xffff0000, \
        ((1<<31) & cpsr) ? 'N' : ' ', \
        ((1<<30) & cpsr) ? 'Z' : ' ', \
        ((1<<29) & cpsr) ? 'C' : ' ', \
        ((1<<28) & cpsr) ? 'V' : ' ' \
        ); \
}

#define TESTINST4(instruction, RMval, RNval, RSval, RD, RM, RN, RS, cvin) \
{ \
    unsigned int out; \
    unsigned int cpsr; \
\
    asm volatile( \
        "msr cpsr_f, %5;" \
        "mov " #RM ",%2;" \
        "mov " #RN ",%3;" \
        "mov " #RS ",%4;" \
        instruction ";" \
        "mov %0," #RD ";" \
        "mrs %1,cpsr;" \
        : "=&r" (out), "=&r" (cpsr) \
        : "r" (RMval), "r" (RNval), "r" (RSval), "r" (gen_cvin(cvin)) \
        : #RD, #RM, #RN, #RS, "cc", "memory" \
    ); \
    printf("%s :: rd 0x%08x rm 0x%08x, rn 0x%08x rs 0x%08x, c:v-in %d, cpsr 0x%08x %c%c%c%c\n", \
        instruction, out, RMval, RNval, RSval, \
        cvin, \
        cpsr & 0xffff0000, \
        ((1<<31) & cpsr) ? 'N' : ' ', \
        ((1<<30) & cpsr) ? 'Z' : ' ', \
        ((1<<29) & cpsr) ? 'C' : ' ', \
        ((1<<28) & cpsr) ? 'V' : ' ' \
        ); \
}

#define TESTINST4_2OUT(instruction, RDval, RD2val, RMval, RSval, RD, RD2, RM, RS, cvin) \
{ \
    unsigned int out; \
    unsigned int out2; \
    unsigned int cpsr; \
\
    asm volatile( \
        "msr cpsr_f, %7;" \
        "mov " #RD ",%3;" \
        "mov " #RD2 ",%4;" \
        "mov " #RM ",%5;" \
        "mov " #RS ",%6;" \
        instruction ";" \
        "mov %0," #RD ";" \
        "mov %1," #RD2 ";" \
        "mrs %2,cpsr;" \
        : "=&r" (out), "=&r" (out2), "=&r" (cpsr) \
        : "r" (RDval), "r" (RD2val), "r" (RMval), "r" (RSval), "r" (gen_cvin(cvin)) \
        : #RD, #RD2, #RM, #RS, "cc", "memory" \
    ); \
    printf("%s :: rd 0x%08x rd2 0x%08x, rm 0x%08x rs 0x%08x, c:v-in %d, cpsr 0x%08x %c%c%c%c\n", \
        instruction, out, out2, RMval, RSval, \
        cvin, \
        cpsr & 0xffff0000, \
        ((1<<31) & cpsr) ? 'N' : ' ', \
        ((1<<30) & cpsr) ? 'Z' : ' ', \
        ((1<<29) & cpsr) ? 'C' : ' ', \
        ((1<<28) & cpsr) ? 'V' : ' ' \
        ); \
}

// Tests misaligned access via PC+$#imm
#define TESTINSTPCMISALIGNED(instruction, RD, label, cvin) \
{ \
    unsigned int out; \
    unsigned int cpsr; \
    asm volatile(\
      ".align 4;" \
      "msr cpsr_f, %2;" \
      "mov " #RD ", #0;" \
      ".align 2;" \
      ".thumb;" \
      ".syntax unified;" \
      "nop;" \
      instruction ";" \
      "b .Lend" #label ";" \
      ".align 4;" \
      #label ": " \
      ".word 0x8191881b;" \
      ".word 0x18fe9c93;" \
      ".word 0x00000000;" \
      ".word 0x00000000;" \
      ".Lend" #label ":" \
      "mov %0, " #RD ";" \
      "mrs %1, cpsr;" \
      : "=&r" (out), "=&r" (cpsr) \
      : "r" (gen_cvin(cvin)) \
      : #RD, "cc", "memory" \
    ); \
    printf("%s :: rd 0x%08x, cpsr 0x%08x %c%c%c%c\n", \
        instruction, out, \
        cpsr & 0xffff0000, \
        ((1<<31) & cpsr) ? 'N' : ' ', \
        ((1<<30) & cpsr) ? 'Z' : ' ', \
        ((1<<29) & cpsr) ? 'C' : ' ', \
        ((1<<28) & cpsr) ? 'V' : ' ' \
        ); \
}

// this one uses d0, s0 and s2 (hardcoded)
#define TESTINSTPCMISALIGNED_DWORDOUT(instruction, label, cvin, extratrash) \
{ \
    unsigned int out; \
    unsigned int out2; \
    unsigned int cpsr; \
    asm volatile(\
      ".align 4;" \
      "msr cpsr_f, %3;" \
      ".align 2;" \
      ".thumb;" \
      ".syntax unified;" \
      "nop;" \
      instruction ";" \
      "b .Lend" #label ";" \
      ".align 4;" \
      #label ": " \
      ".word 0x8191881b;" \
      ".word 0x18fe9c93;" \
      ".word 0x00000000;" \
      ".word 0x00000000;" \
      ".Lend" #label ":" \
      "vmov %0, s0;" \
      "vmov %1, s1;" \
      "mrs %2, cpsr;" \
      : "=&r" (out), "=&r" (out2), "=&r" (cpsr) \
      : "r" (gen_cvin(cvin)) \
      : "cc", "memory", "s0", "s1", extratrash \
    ); \
    printf("%s :: s0 0x%08x s1 0x%08x, cpsr 0x%08x %c%c%c%c\n", \
        instruction, out, out2, \
        cpsr & 0xffff0000, \
        ((1<<31) & cpsr) ? 'N' : ' ', \
        ((1<<30) & cpsr) ? 'Z' : ' ', \
        ((1<<29) & cpsr) ? 'C' : ' ', \
        ((1<<28) & cpsr) ? 'V' : ' ' \
        ); \
}

#define TESTINSTPCMISALIGNED_2OUT(instruction, RD, RD2, label, cvin) \
{ \
    unsigned int out; \
    unsigned int out2; \
    unsigned int cpsr; \
    asm volatile(\
      ".align 4;" \
      "msr cpsr_f, %3;" \
      "mov " #RD ", #0;" \
      "mov " #RD2 ", #0;" \
      ".align 2;" \
      ".thumb;" \
      ".syntax unified;" \
      "nop;" \
      instruction ";" \
      "b .Lend" #label ";" \
      ".align 4;" \
      #label ": " \
      ".word 0x8191881b;" \
      ".word 0x18fe9c93;" \
      ".word 0x00000000;" \
      ".word 0x00000000;" \
      ".Lend" #label ":" \
      "mov %0, " #RD ";" \
      "mov %1, " #RD2 ";" \
      "mrs %2, cpsr;" \
      : "=&r" (out), "=&r" (out2), "=&r" (cpsr) \
      : "r" (gen_cvin(cvin)) \
      : #RD, #RD2, "cc", "memory" \
    ); \
    printf("%s :: rd 0x%08x rd2 0x%08x, cpsr 0x%08x %c%c%c%c\n", \
        instruction, out, out2, \
        cpsr & 0xffff0000, \
        ((1<<31) & cpsr) ? 'N' : ' ', \
        ((1<<30) & cpsr) ? 'Z' : ' ', \
        ((1<<29) & cpsr) ? 'C' : ' ', \
        ((1<<28) & cpsr) ? 'V' : ' ' \
        ); \
}

/* helpers */
#define NOCARRY { int cv = 0; {
#define TESTCARRY { int cv = 0; for (cv = 0; cv < 4; cv++) {
#define TESTCARRYEND }}

void ThumbMain(void)
{
    printf("MOV\n");
    TESTINST2("mov  r0, r1", 1, r0, r1, 0);
    TESTINST2("mov  r0, #0", 0, r0, r1, 0);
    TESTINST2("mov  r0, #1", 0, r0, r1, 0);
    TESTCARRY
    TESTINST2("movs r0, r1", 1, r0, r1, cv);
    TESTINST2("movs r0, r1", 0, r0, r1, cv);
    TESTINST2("movs r0, r1", 0x80000000, r0, r1, cv);
    TESTINST2("movs r0, #0", 0, r0, r1, cv);
    TESTINST2("movs r0, #1", 0, r0, r1, cv);
    TESTCARRYEND

    printf("ADD\n");
    TESTINST3("adds r0, r1, r2", 0, 0, r0, r1, r2, 0);
    TESTINST3("adds r0, r1, r2", 0, 1, r0, r1, r2, 0);
    TESTINST3("adds r0, r1, r2", 1, 0, r0, r1, r2, 0);
    TESTINST3("adds r0, r1, r2", 1, 1, r0, r1, r2, 0);
    TESTINST3("adds r0, r1, r2", 0, -1, r0, r1, r2, 0);
    TESTINST3("adds r0, r1, r2", 1, -1, r0, r1, r2, 0);
    TESTINST3("adds r0, r1, r2", 0x7fffffff, 1, r0, r1, r2, 0);
    TESTINST3("adds r0, r1, r2", 0x80000000, -1, r0, r1, r2, 0);
    TESTINST3("adds r0, r1, r2", 0x80000000, 0, r0, r1, r2, 0);

    printf("ADC\n");
    TESTCARRY
    TESTINST3("adcs r0, r1, r2", 0, 0, r0, r1, r2, cv);
    TESTCARRYEND

    printf("LSL\n");
    TESTINST3("lsl  r0, r1, r2", 0xffffffff, 0, r0, r1, r2, 0);
    TESTINST3("lsl  r0, r1, r2", 0xffffffff, 1, r0, r1, r2, 0);
    TESTINST3("lsl  r0, r1, r2", 0xffffffff, 2, r0, r1, r2, 0);
    TESTINST3("lsl  r0, r1, r2", 0xffffffff, 31, r0, r1, r2, 0);
    TESTINST3("lsl  r0, r1, r2", 0xffffffff, 32, r0, r1, r2, 0);
    TESTINST3("lsl  r0, r1, r2", 0xffffffff, 33, r0, r1, r2, 0);
    TESTINST3("lsl  r0, r1, r2", 0xffffffff, 63, r0, r1, r2, 0);
    TESTINST3("lsl  r0, r1, r2", 0xffffffff, 64, r0, r1, r2, 0);
    TESTINST3("lsl  r0, r1, r2", 0xffffffff, 255, r0, r1, r2, 0);
    TESTINST3("lsl  r0, r1, r2", 0xffffffff, 256, r0, r1, r2, 0);

    TESTINST3("lsl  r0, r1, r2", 0x1, 0, r0, r1, r2, 0);
    TESTINST3("lsl  r0, r1, r2", 0x1, 1, r0, r1, r2, 0);
    TESTINST3("lsl  r0, r1, r2", 0x1, 31, r0, r1, r2, 0);
    TESTINST3("lsl  r0, r1, r2", 0x2, 31, r0, r1, r2, 0);

    printf("LSLS\n");
    TESTCARRY
    TESTINST3("lsls r0, r1, r2", 0xffffffff, 0, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0xffffffff, 1, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0xffffffff, 2, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0xffffffff, 31, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0xffffffff, 32, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0xffffffff, 33, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0xffffffff, 63, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0xffffffff, 64, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0xffffffff, 255, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0xffffffff, 256, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0x1, 0, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0x1, 1, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0x1, 31, r0, r1, r2, cv);
    TESTINST3("lsls r0, r1, r2", 0x2, 31, r0, r1, r2, cv);
    TESTCARRYEND

    printf("LSL immediate\n");
    TESTCARRY
    TESTINST2("lsl  r0, r1, #0", 0xffffffff, r0, r1, cv);
    TESTINST2("lsl  r0, r1, #1", 0xffffffff, r0, r1, cv);
    TESTINST2("lsl  r0, r1, #31", 0xffffffff, r0, r1, cv);
    TESTINST2("lsl  r0, r1, #0", 0x1, r0, r1, cv);
    TESTINST2("lsl  r0, r1, #1", 0x1, r0, r1, cv);
    TESTINST2("lsl  r0, r1, #31", 0x1, r0, r1, cv);
    TESTINST2("lsl  r0, r1, #31", 0x2, r0, r1, cv);
    TESTCARRYEND

    printf("LSR\n");
    TESTINST3("lsr  r0, r1, r2", 0xffffffff, 0, r0, r1, r2, 0);
    TESTINST3("lsr  r0, r1, r2", 0xffffffff, 1, r0, r1, r2, 0);
    TESTINST3("lsr  r0, r1, r2", 0xffffffff, 2, r0, r1, r2, 0);
    TESTINST3("lsr  r0, r1, r2", 0xffffffff, 31, r0, r1, r2, 0);
    TESTINST3("lsr  r0, r1, r2", 0xffffffff, 32, r0, r1, r2, 0);
    TESTINST3("lsr  r0, r1, r2", 0xffffffff, 33, r0, r1, r2, 0);
    TESTINST3("lsr  r0, r1, r2", 0xffffffff, 63, r0, r1, r2, 0);
    TESTINST3("lsr  r0, r1, r2", 0xffffffff, 64, r0, r1, r2, 0);
    TESTINST3("lsr  r0, r1, r2", 0xffffffff, 255, r0, r1, r2, 0);
    TESTINST3("lsr  r0, r1, r2", 0xffffffff, 256, r0, r1, r2, 0);

    printf("LSRS\n");
    TESTCARRY
    TESTINST3("lsrs r0, r1, r2", 0xffffffff, 0, r0, r1, r2, cv);
    TESTINST3("lsrs r0, r1, r2", 0xffffffff, 1, r0, r1, r2, cv);
    TESTINST3("lsrs r0, r1, r2", 0xffffffff, 2, r0, r1, r2, cv);
    TESTINST3("lsrs r0, r1, r2", 0xffffffff, 31, r0, r1, r2, cv);
    TESTINST3("lsrs r0, r1, r2", 0xffffffff, 32, r0, r1, r2, cv);
    TESTINST3("lsrs r0, r1, r2", 0xffffffff, 33, r0, r1, r2, cv);
    TESTINST3("lsrs r0, r1, r2", 0xffffffff, 63, r0, r1, r2, cv);
    TESTINST3("lsrs r0, r1, r2", 0xffffffff, 64, r0, r1, r2, cv);
    TESTINST3("lsrs r0, r1, r2", 0xffffffff, 255, r0, r1, r2, cv);
    TESTCARRYEND

    printf("LSR immediate\n");
    TESTINST2("lsr  r0, r1, #0", 0xffffffff, r0, r1, 0);
    TESTINST2("lsr  r0, r1, #1", 0xffffffff, r0, r1, 0);
    TESTINST2("lsr  r0, r1, #31", 0xffffffff, r0, r1, 0);
    TESTINST2("lsr  r0, r1, #32", 0xffffffff, r0, r1, 0);
    TESTINST2("lsr  r0, r1, #16", 0x00010000, r0, r1, 0);
    TESTINST2("lsr  r0, r1, #17", 0x00010000, r0, r1, 0);
    TESTINST2("lsr  r0, r1, #18", 0x00010000, r0, r1, 0);

    printf("LSRS immediate\n");
    TESTCARRY
    TESTINST2("lsrs r0, r1, #0", 0xffffffff, r0, r1, cv);
    TESTINST2("lsrs r0, r1, #1", 0xffffffff, r0, r1, cv);
    TESTINST2("lsrs r0, r1, #31", 0xffffffff, r0, r1, cv);
    TESTINST2("lsrs r0, r1, #32", 0xffffffff, r0, r1, cv);
    TESTINST2("lsrs r0, r1, #16", 0x00010000, r0, r1, cv);
    TESTINST2("lsrs r0, r1, #17", 0x00010000, r0, r1, cv);
    TESTINST2("lsrs r0, r1, #18", 0x00010000, r0, r1, cv);
    TESTCARRYEND

    printf("ASR\n");
    TESTCARRY
    TESTINST3("asr  r0, r1, r2", 0xffffffff, 0, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0xffffffff, 1, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0xffffffff, 2, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0xffffffff, 31, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0xffffffff, 32, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0xffffffff, 33, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0xffffffff, 63, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0xffffffff, 64, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0xffffffff, 255, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0xffffffff, 256, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0x7fffffff, 0, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0x7fffffff, 1, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0x7fffffff, 2, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0x7fffffff, 31, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0x7fffffff, 32, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0x7fffffff, 33, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0x7fffffff, 63, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0x7fffffff, 64, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0x7fffffff, 255, r0, r1, r2, cv);
    TESTINST3("asr  r0, r1, r2", 0x7fffffff, 256, r0, r1, r2, cv);
    TESTCARRYEND

    printf("ASRS\n");
    TESTCARRY
    TESTINST3("asrs r0, r1, r2", 0xffffffff, 0, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0xffffffff, 1, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0xffffffff, 2, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0xffffffff, 31, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0xffffffff, 32, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0xffffffff, 33, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0xffffffff, 63, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0xffffffff, 64, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0xffffffff, 255, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0xffffffff, 256, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x7fffffff, 0, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x7fffffff, 1, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x7fffffff, 2, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x7fffffff, 31, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x7fffffff, 32, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x7fffffff, 33, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x7fffffff, 63, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x7fffffff, 64, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x7fffffff, 255, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x7fffffff, 256, r0, r1, r2, cv);
    TESTCARRYEND

    TESTCARRY
    TESTINST3("asrs r0, r1, r2", 0x8, 0, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x8, 1, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x8, 2, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x8, 3, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x8, 4, r0, r1, r2, cv);
    TESTINST3("asrs r0, r1, r2", 0x8, 5, r0, r1, r2, cv);
    TESTCARRYEND

    TESTINST3("asrs r0, r1, r2", 0x80000001, 1, r0, r1, r2, 0);
    TESTINST3("asrs r0, r1, r2", 0x80000001, 2, r0, r1, r2, 0);

    printf("ASR immediate\n");
    TESTINST2("asr  r0, r1, #0", 0xffffffff, r0, r1, 0);
    TESTINST2("asr  r0, r1, #1", 0xffffffff, r0, r1, 0);
    TESTINST2("asr  r0, r1, #31", 0xffffffff, r0, r1, 0);
    TESTINST2("asr  r0, r1, #32", 0xffffffff, r0, r1, 0);
    TESTINST2("asr  r0, r1, #0", 0x7fffffff, r0, r1, 0);
    TESTINST2("asr  r0, r1, #1", 0x7fffffff, r0, r1, 0);
    TESTINST2("asr  r0, r1, #31", 0x7fffffff, r0, r1, 0);
    TESTINST2("asr  r0, r1, #32", 0x7fffffff, r0, r1, 0);
    TESTINST2("asr  r0, r1, #16", 0x00010000, r0, r1, 0);
    TESTINST2("asr  r0, r1, #17", 0x00010000, r0, r1, 0);
    TESTINST2("asr  r0, r1, #18", 0x00010000, r0, r1, 0);

    printf("ASRS immediate\n");
    TESTCARRY
    TESTINST2("asrs r0, r1, #0", 0xffffffff, r0, r1, cv);
    TESTINST2("asrs r0, r1, #1", 0xffffffff, r0, r1, cv);
    TESTINST2("asrs r0, r1, #31", 0xffffffff, r0, r1, cv);
    TESTINST2("asrs r0, r1, #32", 0xffffffff, r0, r1, cv);
    TESTINST2("asrs r0, r1, #0", 0x7fffffff, r0, r1, cv);
    TESTINST2("asrs r0, r1, #1", 0x7fffffff, r0, r1, cv);
    TESTINST2("asrs r0, r1, #31", 0x7fffffff, r0, r1, cv);
    TESTINST2("asrs r0, r1, #32", 0x7fffffff, r0, r1, cv);
    TESTINST2("asrs r0, r1, #16", 0x00010000, r0, r1, cv);
    TESTINST2("asrs r0, r1, #17", 0x00010000, r0, r1, cv);
    TESTINST2("asrs r0, r1, #18", 0x00010000, r0, r1, cv);
    TESTCARRYEND

    printf("MUL\n");
    TESTINST3("mul  r0, r1, r2", 0, 0, r0, r1, r2, 0);
    TESTINST3("mul  r0, r1, r2", 0xffffffff, 0, r0, r1, r2, 0);
    TESTINST3("mul  r0, r1, r2", 0, 0xffffffff, r0, r1, r2, 0);
    TESTINST3("mul  r0, r1, r2", 0xffffffff, 0xffffffff, r0, r1, r2, 0);
    TESTINST3("mul  r0, r1, r2", 0x7fffffff, 0x7fffffff, r0, r1, r2, 0);
    TESTINST3("mul  r0, r1, r2", 0x0000ffff, 0x0000ffff, r0, r1, r2, 0);

    //
    // Newer tests begin here.
    //

    printf("CMP-16 0x10a\n");
    TESTCARRY
    TESTINST3("cmp r3, r6", 0,          0,           r6/*fake*/, r3, r6, 0);
    TESTINST3("cmp r3, r6", 1,          0,           r6/*fake*/, r3, r6, 0);
    TESTINST3("cmp r3, r6", 0,          1,           r6/*fake*/, r3, r6, 0);
    TESTINST3("cmp r3, r6", -1,         0,           r6/*fake*/, r3, r6, 0);
    TESTINST3("cmp r3, r6", 0,          -1,          r6/*fake*/, r3, r6, 0);
    TESTINST3("cmp r3, r6", 0,          0x80000000,  r6/*fake*/, r3, r6, 0);
    TESTINST3("cmp r3, r6", 0x80000000, 0,           r6/*fake*/, r3, r6, 0);
    TESTCARRYEND

    printf("CMN-16 0x10a\n");
    TESTCARRY
    TESTINST3("cmn r3, r6", 0,          0,           r6/*fake*/, r3, r6, 0);
    TESTINST3("cmn r3, r6", 1,          0,           r6/*fake*/, r3, r6, 0);
    TESTINST3("cmn r3, r6", 0,          1,           r6/*fake*/, r3, r6, 0);
    TESTINST3("cmn r3, r6", -1,         0,           r6/*fake*/, r3, r6, 0);
    TESTINST3("cmn r3, r6", 0,          -1,          r6/*fake*/, r3, r6, 0);
    TESTINST3("cmn r3, r6", 0,          0x80000000,  r6/*fake*/, r3, r6, 0);
    TESTINST3("cmn r3, r6", 0x80000000, 0,           r6/*fake*/, r3, r6, 0);
    TESTCARRYEND

    printf("TST-16 0x108\n");
    TESTCARRY
    TESTINST3("tst r3, r6", 0,          0,           r6/*fake*/, r3, r6, cv);
    TESTINST3("tst r3, r6", 1,          0,           r6/*fake*/, r3, r6, cv);
    TESTINST3("tst r3, r6", 0,          1,           r6/*fake*/, r3, r6, cv);
    TESTINST3("tst r3, r6", 1,          1,           r6/*fake*/, r3, r6, cv);
    TESTINST3("tst r3, r6", -1,         0,           r6/*fake*/, r3, r6, cv);
    TESTINST3("tst r3, r6", 0,          -1,          r6/*fake*/, r3, r6, cv);
    TESTINST3("tst r3, r6", -1,         -1,          r6/*fake*/, r3, r6, cv);
    TESTCARRYEND

    printf("NEGS-16 0x109\n");
    TESTINST2("negs r0, r1", 1, r0, r1, 0);
    TESTCARRY
    TESTINST2("negs r0, r1", 1, r0, r1, cv);
    TESTINST2("negs r0, r1", 0, r0, r1, cv);
    TESTINST2("negs r0, r1", 0x80000000, r0, r1, cv);
    TESTINST2("negs r0, r1", 0x80000001, r0, r1, cv);
    TESTINST2("negs r0, r1", 0xFFFFFFFF, r0, r1, cv);
    TESTINST2("negs r0, r1", 0x7FFFFFFF, r0, r1, cv);
    TESTCARRYEND

    printf("MVNS-16 0x10F\n");
    TESTINST2("mvns r0, r1", 1, r0, r1, 0);
    TESTCARRY
    TESTINST2("mvns r0, r1", 1, r0, r1, cv);
    TESTINST2("mvns r0, r1", 0, r0, r1, cv);
    TESTINST2("mvns r0, r1", 0x80000000, r0, r1, cv);
    TESTINST2("mvns r0, r1", 0x80000001, r0, r1, cv);
    TESTINST2("mvns r0, r1", 0xFFFFFFFF, r0, r1, cv);
    TESTINST2("mvns r0, r1", 0x7FFFFFFF, r0, r1, cv);
    TESTCARRYEND

    printf("ORRS-16 0x10C\n");
    TESTCARRY
    TESTINST2x("orrs r1, r2", 0x31415927, 0x27181728, r1, r2, cv);
    TESTINST2x("orrs r1, r2", 0x00000000, 0x00000000, r1, r2, cv);
    TESTINST2x("orrs r1, r2", 0x00000001, 0x00000000, r1, r2, cv);
    TESTINST2x("orrs r1, r2", 0x00000000, 0x00000001, r1, r2, cv);
    TESTINST2x("orrs r1, r2", 0x80000000, 0x00000000, r1, r2, cv);
    TESTINST2x("orrs r1, r2", 0x00000000, 0x80000000, r1, r2, cv);
    TESTINST2x("orrs r1, r2", 0x80000000, 0x80000000, r1, r2, cv);
    TESTCARRYEND

    printf("ANDS-16 0x100\n");
    TESTCARRY
    TESTINST2x("ands r1, r2", 0x31415927, 0x27181728, r1, r2, cv);
    TESTINST2x("ands r1, r2", 0x00000000, 0x00000000, r1, r2, cv);
    TESTINST2x("ands r1, r2", 0x00000001, 0x00000000, r1, r2, cv);
    TESTINST2x("ands r1, r2", 0x00000000, 0x00000001, r1, r2, cv);
    TESTINST2x("ands r1, r2", 0x80000000, 0x00000000, r1, r2, cv);
    TESTINST2x("ands r1, r2", 0x00000000, 0x80000000, r1, r2, cv);
    TESTINST2x("ands r1, r2", 0x80000000, 0x80000000, r1, r2, cv);
    TESTCARRYEND

    printf("EORS-16 0x101\n");
    TESTCARRY
    TESTINST2x("eors r1, r2", 0x31415927, 0x27181728, r1, r2, cv);
    TESTINST2x("eors r1, r2", 0x00000000, 0x00000000, r1, r2, cv);
    TESTINST2x("eors r1, r2", 0x00000001, 0x00000000, r1, r2, cv);
    TESTINST2x("eors r1, r2", 0x00000000, 0x00000001, r1, r2, cv);
    TESTINST2x("eors r1, r2", 0x80000000, 0x00000000, r1, r2, cv);
    TESTINST2x("eors r1, r2", 0x00000000, 0x80000000, r1, r2, cv);
    TESTINST2x("eors r1, r2", 0x80000000, 0x80000000, r1, r2, cv);
    TESTCARRYEND

    printf("MULS-16 0x10d\n");
    TESTCARRY
    TESTINST2x("muls r1, r2", 0x31415927, 0x27181728, r1, r2, cv);
    TESTINST2x("muls r1, r2", 0x00000000, 0x00000000, r1, r2, cv);
    TESTINST2x("muls r1, r2", 0x00000001, 0x00000000, r1, r2, cv);
    TESTINST2x("muls r1, r2", 0x00000000, 0x00000001, r1, r2, cv);
    TESTINST2x("muls r1, r2", 0x80000000, 0x00000000, r1, r2, cv);
    TESTINST2x("muls r1, r2", 0x00000000, 0x80000000, r1, r2, cv);
    TESTINST2x("muls r1, r2", 0x80000000, 0x80000000, r1, r2, cv);
    TESTCARRYEND

    printf("BICS-16 0x10E\n");
    TESTCARRY
    TESTINST2x("bics r1, r2", 0x31415927, 0x27181728, r1, r2, cv);
    TESTINST2x("bics r1, r2", 0x00000000, 0x00000000, r1, r2, cv);
    TESTINST2x("bics r1, r2", 0x00000001, 0x00000000, r1, r2, cv);
    TESTINST2x("bics r1, r2", 0x00000000, 0x00000001, r1, r2, cv);
    TESTINST2x("bics r1, r2", 0x80000000, 0x00000000, r1, r2, cv);
    TESTINST2x("bics r1, r2", 0x00000000, 0x80000000, r1, r2, cv);
    TESTINST2x("bics r1, r2", 0x80000000, 0x80000000, r1, r2, cv);
    TESTCARRYEND

    printf("ADCS-16 0x105\n");
    TESTCARRY
    TESTINST2x("adcs r1, r2", 0x31415927, 0x27181728, r1, r2, cv);
    TESTINST2x("adcs r1, r2", 0x00000000, 0x00000000, r1, r2, cv);
    TESTINST2x("adcs r1, r2", 0x00000001, 0x00000000, r1, r2, cv);
    TESTINST2x("adcs r1, r2", 0x00000000, 0x00000001, r1, r2, cv);
    TESTINST2x("adcs r1, r2", 0x80000000, 0x00000000, r1, r2, cv);
    TESTINST2x("adcs r1, r2", 0x00000000, 0x80000000, r1, r2, cv);
    TESTINST2x("adcs r1, r2", 0x80000000, 0x80000000, r1, r2, cv);
    TESTCARRYEND

    printf("SBCS-16 0x100\n");
    TESTCARRY
    TESTINST2x("sbcs r1, r2", 0x31415927, 0x27181728, r1, r2, cv);
    TESTINST2x("sbcs r1, r2", 0x00000000, 0x00000000, r1, r2, cv);
    TESTINST2x("sbcs r1, r2", 0x00000001, 0x00000000, r1, r2, cv);
    TESTINST2x("sbcs r1, r2", 0x00000000, 0x00000001, r1, r2, cv);
    TESTINST2x("sbcs r1, r2", 0x80000000, 0x00000000, r1, r2, cv);
    TESTINST2x("sbcs r1, r2", 0x00000000, 0x80000000, r1, r2, cv);
    TESTINST2x("sbcs r1, r2", 0x80000000, 0x80000000, r1, r2, cv);
    TESTCARRYEND

    printf("LSLS-16 0x102\n");
    TESTCARRY
    TESTINST2x("lsls r1, r2", 0x31415927, 0x00000000, r1, r2, cv);
    TESTINST2x("lsls r1, r2", 0x31415927, 0x00000001, r1, r2, cv);
    TESTINST2x("lsls r1, r2", 0x31415927, 0x00000002, r1, r2, cv);
    TESTINST2x("lsls r1, r2", 0x31415927, 0x0000000F, r1, r2, cv);
    TESTINST2x("lsls r1, r2", 0x31415927, 0x00000010, r1, r2, cv);
    TESTINST2x("lsls r1, r2", 0x31415927, 0x0000001F, r1, r2, cv);
    TESTINST2x("lsls r1, r2", 0x31415927, 0x00000020, r1, r2, cv);
    TESTINST2x("lsls r1, r2", 0x31415927, 0x00000021, r1, r2, cv);
    TESTCARRYEND

    printf("LSRS-16 0x103\n");
    TESTCARRY
    TESTINST2x("lsrs r1, r2", 0x31415927, 0x00000000, r1, r2, cv);
    TESTINST2x("lsrs r1, r2", 0x31415927, 0x00000001, r1, r2, cv);
    TESTINST2x("lsrs r1, r2", 0x31415927, 0x00000002, r1, r2, cv);
    TESTINST2x("lsrs r1, r2", 0x31415927, 0x0000000F, r1, r2, cv);
    TESTINST2x("lsrs r1, r2", 0x31415927, 0x00000010, r1, r2, cv);
    TESTINST2x("lsrs r1, r2", 0x31415927, 0x0000001F, r1, r2, cv);
    TESTINST2x("lsrs r1, r2", 0x31415927, 0x00000020, r1, r2, cv);
    TESTINST2x("lsrs r1, r2", 0x31415927, 0x00000021, r1, r2, cv);
    TESTCARRYEND

    printf("ASRS-16 0x104\n");
    TESTCARRY
    TESTINST2x("asrs r1, r2", 0x31415927, 0x00000000, r1, r2, cv);
    TESTINST2x("asrs r1, r2", 0x91415927, 0x00000001, r1, r2, cv);
    TESTINST2x("asrs r1, r2", 0x31415927, 0x00000002, r1, r2, cv);
    TESTINST2x("asrs r1, r2", 0x91415927, 0x0000000F, r1, r2, cv);
    TESTINST2x("asrs r1, r2", 0x31415927, 0x00000010, r1, r2, cv);
    TESTINST2x("asrs r1, r2", 0x91415927, 0x0000001F, r1, r2, cv);
    TESTINST2x("asrs r1, r2", 0x31415927, 0x00000020, r1, r2, cv);
    TESTINST2x("asrs r1, r2", 0x91415927, 0x00000021, r1, r2, cv);
    TESTCARRYEND

    printf("RORS-16 0x107\n");
    TESTCARRY
    TESTINST2x("rors r1, r2", 0x31415927, 0x00000000, r1, r2, cv);
    TESTINST2x("rors r1, r2", 0x31415927, 0x00000001, r1, r2, cv);
    TESTINST2x("rors r1, r2", 0x31415927, 0x00000002, r1, r2, cv);
    TESTINST2x("rors r1, r2", 0x31415927, 0x0000000F, r1, r2, cv);
    TESTINST2x("rors r1, r2", 0x31415927, 0x00000010, r1, r2, cv);
    TESTINST2x("rors r1, r2", 0x31415927, 0x0000001F, r1, r2, cv);
    TESTINST2x("rors r1, r2", 0x31415927, 0x00000020, r1, r2, cv);
    TESTINST2x("rors r1, r2", 0x31415927, 0x00000021, r1, r2, cv);
    TESTCARRYEND

    printf("ADD(HI)-16\n");
    TESTCARRY
    TESTINST2x("add r5, r12", 0x31415927, 0x12345678, r5, r12, cv);
    TESTINST2x("add r4, r9 ", 0x31415927, 0x12345678, r4, r9,  cv);
    TESTCARRYEND

    printf("CMP(HI)-16 0x10a\n");
    TESTCARRY
    TESTINST3("cmp r5, r12", 0,          0,           r12/*fake*/, r5, r12, 0);
    TESTINST3("cmp r5, r12", 1,          0,           r12/*fake*/, r5, r12, 0);
    TESTINST3("cmp r5, r12", 0,          1,           r12/*fake*/, r5, r12, 0);
    TESTINST3("cmp r5, r12", -1,         0,           r12/*fake*/, r5, r12, 0);
    TESTINST3("cmp r5, r12", 0,          -1,          r12/*fake*/, r5, r12, 0);
    TESTINST3("cmp r5, r12", 0,          0x80000000,  r12/*fake*/, r5, r12, 0);
    TESTINST3("cmp r5, r12", 0x80000000, 0,           r12/*fake*/, r5, r12, 0);
    TESTCARRYEND

    printf("MOV(HI)-16\n");
    TESTCARRY
    TESTINST2x("mov r5, r12", 0x31415927, 0x12345678, r5, r12, cv);
    TESTINST2x("mov r4, r9 ", 0x31415927, 0x12345678, r4, r9,  cv);
    TESTCARRYEND

    printf("ADDS-16 Rd, Rn, #imm3\n");
    TESTCARRY
    TESTINST2x("adds r1, r2, #1", 0x31415927, 0x27181728, r1, r2, cv);
    TESTINST2x("adds r1, r2, #7", 0x31415927, 0x97181728, r1, r2, cv);
    TESTCARRYEND

    printf("ADDS-16 Rd, Rn, Rm\n");
    TESTCARRY
    TESTINST3("adds r1, r2, r3", 0x31415927, 0x27181728, r1, r2, r3, cv);
    TESTINST3("adds r1, r2, r3", 0x31415927, 0x97181728, r1, r2, r3, cv);
    TESTINST3("adds r1, r2, r3", 0,          0,          r1, r2, r3, cv);
    TESTINST3("adds r1, r2, r3", 1,          0,          r1, r2, r3, cv);
    TESTINST3("adds r1, r2, r3", 0,          1,          r1, r2, r3, cv);
    TESTINST3("adds r1, r2, r3", -1,         0,          r1, r2, r3, cv);
    TESTINST3("adds r1, r2, r3", 0,          -1,         r1, r2, r3, cv);
    TESTINST3("adds r1, r2, r3", 0,          0x80000000, r1, r2, r3, cv);
    TESTINST3("adds r1, r2, r3", 0x80000000, 0,          r1, r2, r3, cv);
    TESTINST3("adds r1, r2, r3", 0x80000000, 0x80000000, r1, r2, r3, cv);
    TESTINST3("adds r1, r2, r3", 0x7fffffff, 0x80000000, r1, r2, r3, cv);
    TESTINST3("adds r1, r2, r3", 0x80000000, 0x7fffffff, r1, r2, r3, cv);
    TESTINST3("adds r1, r2, r3", 0x7fffffff, 0x7fffffff, r1, r2, r3, cv);
    TESTCARRYEND

    printf("SUBS-16 Rd, Rn, Rm\n");
    TESTCARRY
    TESTINST3("subs r1, r2, r3", 0x31415927, 0x27181728, r1, r2, r3, cv);
    TESTINST3("subs r1, r2, r3", 0x31415927, 0x97181728, r1, r2, r3, cv);
    TESTINST3("subs r1, r2, r3", 0,          0,          r1, r2, r3, cv);
    TESTINST3("subs r1, r2, r3", 1,          0,          r1, r2, r3, cv);
    TESTINST3("subs r1, r2, r3", 0,          1,          r1, r2, r3, cv);
    TESTINST3("subs r1, r2, r3", -1,         0,          r1, r2, r3, cv);
    TESTINST3("subs r1, r2, r3", 0,          -1,         r1, r2, r3, cv);
    TESTINST3("subs r1, r2, r3", 0,          0x80000000, r1, r2, r3, cv);
    TESTINST3("subs r1, r2, r3", 0x80000000, 0,          r1, r2, r3, cv);
    TESTINST3("subs r1, r2, r3", 0x80000000, 0x80000000, r1, r2, r3, cv);
    TESTINST3("subs r1, r2, r3", 0x7fffffff, 0x80000000, r1, r2, r3, cv);
    TESTINST3("subs r1, r2, r3", 0x80000000, 0x7fffffff, r1, r2, r3, cv);
    TESTINST3("subs r1, r2, r3", 0x7fffffff, 0x7fffffff, r1, r2, r3, cv);
    TESTCARRYEND

    printf("ADDS-16 Rn, #uimm8\n");
    TESTCARRY
    TESTINST1x("adds r1, #0  ", 0x31415927, r1, cv);
    TESTINST1x("adds r1, #255", 0x31415927, r1, cv);
    TESTINST1x("adds r1, #0  ", 0x91415927, r1, cv);
    TESTINST1x("adds r1, #255", 0x91415927, r1, cv);
    TESTCARRYEND

    printf("SUBS-16 Rn, #uimm8\n");
    TESTCARRY
    TESTINST1x("subs r1, #0  ", 0x31415927, r1, cv);
    TESTINST1x("subs r1, #255", 0x31415927, r1, cv);
    TESTINST1x("subs r1, #0  ", 0x91415927, r1, cv);
    TESTINST1x("subs r1, #255", 0x91415927, r1, cv);
    TESTCARRYEND

    printf("CMP-16 Rn, #uimm8\n");
    TESTCARRY
    TESTINST1x("cmp r1, #0x80  ", 0x00000080, r1, cv);
    TESTINST1x("cmp r1, #0x7f  ", 0x00000080, r1, cv);
    TESTINST1x("cmp r1, #0x81  ", 0x00000080, r1, cv);
    TESTINST1x("cmp r1, #0x80  ", 0xffffff80, r1, cv);
    TESTINST1x("cmp r1, #0x7f  ", 0xffffff80, r1, cv);
    TESTINST1x("cmp r1, #0x81  ", 0xffffff80, r1, cv);
    TESTINST1x("cmp r1, #0x01  ", 0x80000000, r1, cv);
    TESTCARRYEND

    printf("MOVS-16 Rn, #uimm8\n");
    TESTCARRY
    TESTINST1x("movs r1, #0   ", 0x31415927, r1, cv);
    TESTINST1x("movs r1, #0x7f", 0x31415927, r1, cv);
    TESTINST1x("movs r1, #0x80", 0x31415927, r1, cv);
    TESTINST1x("movs r1, #0x81", 0x31415927, r1, cv);
    TESTINST1x("movs r1, #0xff", 0x31415927, r1, cv);
    TESTCARRYEND

    printf("LSLS-16 Rd, Rm, imm5\n");
    TESTCARRY
    TESTINST2("lsls r1, r2, #0   ", 0x31415927, r1, r2, cv);
    TESTINST2("lsls r1, r2, #1   ", 0x31415927, r1, r2, cv);
    TESTINST2("lsls r1, r2, #2   ", 0x31415927, r1, r2, cv);
    TESTINST2("lsls r1, r2, #0xF ", 0x31415927, r1, r2, cv);
    TESTINST2("lsls r1, r2, #0x10", 0x31415927, r1, r2, cv);
    TESTINST2("lsls r1, r2, #0x1F", 0x31415927, r1, r2, cv);
    TESTCARRYEND

    printf("LSRS-16 Rd, Rm, imm5\n");
    TESTCARRY
    TESTINST2("lsrs r1, r2, #0   ", 0x31415927, r1, r2, cv);
    TESTINST2("lsrs r1, r2, #1   ", 0x31415927, r1, r2, cv);
    TESTINST2("lsrs r1, r2, #2   ", 0x31415927, r1, r2, cv);
    TESTINST2("lsrs r1, r2, #0xF ", 0x31415927, r1, r2, cv);
    TESTINST2("lsrs r1, r2, #0x10", 0x31415927, r1, r2, cv);
    TESTINST2("lsrs r1, r2, #0x1F", 0x31415927, r1, r2, cv);
    TESTCARRYEND

    printf("ASRS-16 Rd, Rm, imm5\n");
    TESTCARRY
    TESTINST2("asrs r1, r2, #0   ", 0x31415927, r1, r2, cv);
    TESTINST2("asrs r1, r2, #1   ", 0x91415927, r1, r2, cv);
    TESTINST2("asrs r1, r2, #2   ", 0x31415927, r1, r2, cv);
    TESTINST2("asrs r1, r2, #0xF ", 0x91415927, r1, r2, cv);
    TESTINST2("asrs r1, r2, #0x10", 0x31415927, r1, r2, cv);
    TESTINST2("asrs r1, r2, #0x1F", 0x91415927, r1, r2, cv);
    TESTCARRYEND
}
