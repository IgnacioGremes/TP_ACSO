#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "shell.h"

void process_instruction()
{
    // Fetch the 32-bit instruction from memory at PC
    uint32_t instr = mem_read_32(CURRENT_STATE.PC);
    
    printf("%x",instr);
    // Data Processing: ADDS Xd, Xn, imm (possible shift) <---- falta hacer el shift
    if ((instr >> 24) == 0xB1) {
        // Extract fields for decoding (not all used yet, but useful for expansion)
        uint32_t imm12 = (instr >> 10) & 4095; // Bits 21-10
        uint32_t Rn = (instr >> 5) & 31;  // Bits 9-5
        uint32_t Rd = instr & 31;         // Bits 4-0

        // Shift case
        if (((instr >> 22) & 3) == 0x1) {
            imm12 = imm12 << 12;
        }
        
        // Get source values, treating X31 as 0
        int64_t val_Rn = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t result = val_Rn + imm12;

        // Write result to Rd unless it's X31
        if (Rd != 31) {
            NEXT_STATE.REGS[Rd] = result;
        }

        // Set flags: N (negative), Z (zero)
        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;       // Bit 63 = 1
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

        // Increment PC
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
    
    // Data Processing: ADDS Xd, Xn, Xm
    else if ((instr >> 21) == 0x558) {
        // Extract fields for decoding (not all used yet, but useful for expansion)
        uint32_t Rm = (instr >> 16) & 31; // Bits 20-16
        uint32_t Rn = (instr >> 5) & 31;  // Bits 9-5
        uint32_t Rd = instr & 31;         // Bits 4-0

        
        // Get source values, treating X31 as 0
        int64_t val_Rn = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t val_Rm = (Rm == 31) ? 0 : CURRENT_STATE.REGS[Rm];
        int64_t result = val_Rn + val_Rm;

        // Write result to Rd unless it's X31
        if (Rd != 31) {
            NEXT_STATE.REGS[Rd] = result;
        }

        // Set flags: N (negative), Z (zero)
        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;       // Bit 63 = 1
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

        // Increment PC
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

    // SUBS: Xd, Xn, Xm (extended register) and CMP: Xn, Xm (extended register)
    else if ((instr >> 21) == 0x758) {
        // Extract fields for decoding (not all used yet, but useful for expansion)
        uint32_t Rm = (instr >> 16) & 31; // Bits 20-16
        uint32_t Rn = (instr >> 5) & 31;  // Bits 9-5
        uint32_t Rd = instr & 31;         // Bits 4-0

        
        // Get source values, treating X31 as 0
        int64_t val_Rn = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t val_Rm = (Rm == 31) ? 0 : CURRENT_STATE.REGS[Rm];
        int64_t result = val_Rn - val_Rm;

        // Write result to Rd unless it's X31
        if (Rd != 31) {
            NEXT_STATE.REGS[Rd] = result;
        }

        // Set flags: N (negative), Z (zero)
        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;       // Bit 63 = 1
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

        // Increment PC
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

    // SUBS: Xd, Xn, imm12 (immediate) and CMP: Xn, imm12 (immediate)
    else if ((instr >> 24) == 0xF1) {

        // Extract fields for decoding (not all used yet, but useful for expansion)
        uint32_t imm12 = (instr >> 10) & 4095; // Bits 21-10
        uint32_t Rn = (instr >> 5) & 31;  // Bits 9-5
        uint32_t Rd = instr & 31;         // Bits 4-0

        // Shift case
        if (((instr >> 22) & 3) == 0x1) {
            imm12 = imm12 << 12;
        }

        // Get source values, treating X31 as 0
        int64_t val_Rn = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t result = val_Rn - imm12;

        // Write result to Rd unless it's X31
        if (Rd != 31) {
            NEXT_STATE.REGS[Rd] = result;
        }

        // Set flags: N (negative), Z (zero)
        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;       // Bit 63 = 1
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

        // Increment PC
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

    // ANDS: Xd, Xn, Xm (shifted register)
    else if((instr >> 21) == 0x750) {

        uint32_t Rm = (instr >> 16) & 31; // bits 20-16
        uint32_t Rn = (instr >> 5) & 31; // bits 9-5
        uint32_t Rd = instr & 31; // bits 4-0

        int64_t val_Rm = (Rm == 31) ? 0 : CURRENT_STATE.REGS[Rm];
        int64_t val_Rn = (Rm == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t result = val_Rn & val_Rm;

        if (Rd != 31){
            NEXT_STATE.REGS[Rd] = result;
        }

        // Set flags: N (negative), Z (zero)
        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;       // Bit 63 = 1
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

        // Increment PC
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

    // EOR: Xd, Xn, Xm (shifted register)
    else if((instr >> 21) == 0x650) {

        uint32_t Rm = (instr >> 16) & 31; // bits 20-16
        uint32_t Rn = (instr >> 5) & 31; // bits 9-5
        uint32_t Rd = instr & 31; // bits 4-0

        int64_t val_Rm = (Rm == 31) ? 0 : CURRENT_STATE.REGS[Rm];
        int64_t val_Rn = (Rm == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t result = val_Rn ^ val_Rm;

        if (Rd != 31){
            NEXT_STATE.REGS[Rd] = result;
        }

        // Set flags: N (negative), Z (zero)
        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;       // Bit 63 = 1
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

        // Increment PC
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

    // OOR: Xd, Xn, Xm (shifted register)
    else if((instr >> 21) == 0x550) {

        uint32_t Rm = (instr >> 16) & 31; // bits 20-16
        uint32_t Rn = (instr >> 5) & 31; // bits 9-5
        uint32_t Rd = instr & 31; // bits 4-0

        int64_t val_Rm = (Rm == 31) ? 0 : CURRENT_STATE.REGS[Rm];
        int64_t val_Rn = (Rm == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t result = val_Rn | val_Rm;

        if (Rd != 31){
            NEXT_STATE.REGS[Rd] = result;
        }

        // Set flags: N (negative), Z (zero)
        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;       // Bit 63 = 1
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

        // Increment PC
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

    // HLT
    else if ((instr >> 21) == 0x6A2) {
        RUN_BIT = FALSE;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4; 
    }
    
    // Conditional Branch: B.cond
    else if ((instr & 0xFF000000) == 0x54000000) {
        // Extract condition and offset
        uint32_t cond = instr & 0xF;                  // Bits 3-0
        int32_t offset = ((instr >> 5) & 0x7FFFF) << 2; // Bits 23-5, shifted by 2
        if (offset & 0x80000) {                       // Sign-extend 19-bit offset
            offset |= 0xFFF00000;
        }

        // Evaluate condition
        int branch = FALSE;
        switch (cond) {
            case 0x0: branch = (CURRENT_STATE.FLAG_Z == 1); break; // BEQ
            case 0x1: branch = (CURRENT_STATE.FLAG_Z == 0); break; // BNE
            // Add more conditions if needed (limited by N and Z flags)
            default: break;
        }

        // Set next PC
        NEXT_STATE.PC = branch ? (CURRENT_STATE.PC + offset) : (CURRENT_STATE.PC + 4);
    }
    // Unimplemented instruction: halt simulator
    else {
        printf("Unimplemented instruction: 0x%08x at PC: 0x%" PRIx64 "\n", instr, CURRENT_STATE.PC);
        RUN_BIT = FALSE;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4; // Ensure PC advances even on halt
    }
}

