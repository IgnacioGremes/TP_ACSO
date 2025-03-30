#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include "shell.h"

void process_instruction()
{
    // Fetch the 32-bit instruction from memory at PC
    uint32_t instr = mem_read_32(CURRENT_STATE.PC);
    
    // ADDS Xd, Xn, imm (immediate)
    if ((instr >> 24) == 0xB1) {

        uint32_t imm12 = (instr >> 10) & 4095; // Bits 21-10
        uint32_t Rn = (instr >> 5) & 31;  // Bits 9-5
        uint32_t Rd = instr & 31;         // Bits 4-0

        // Shift case
        if (((instr >> 22) & 3) == 0x1) {
            imm12 = imm12 << 12;
        }
        
        int64_t val_Rn = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t result = val_Rn + imm12;

        if (Rd != 31) {
            NEXT_STATE.REGS[Rd] = result;
        }

        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
    
    // ADDS Xd, Xn, Xm (extended register)
    else if ((instr >> 21) == 0x558) {

        uint32_t Rm = (instr >> 16) & 31; // Bits 20-16
        uint32_t Rn = (instr >> 5) & 31;  // Bits 9-5
        uint32_t Rd = instr & 31;         // Bits 4-0

        
        int64_t val_Rn = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t val_Rm = (Rm == 31) ? 0 : CURRENT_STATE.REGS[Rm];
        int64_t result = val_Rn + val_Rm;

        if (Rd != 31) {
            NEXT_STATE.REGS[Rd] = result;
        }

        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

    // SUBS: Xd, Xn, Xm (extended register) and CMP: Xn, Xm (extended register)
    else if ((instr >> 21) == 0x758) {

        uint32_t Rm = (instr >> 16) & 31; // Bits 20-16
        uint32_t Rn = (instr >> 5) & 31;  // Bits 9-5
        uint32_t Rd = instr & 31;         // Bits 4-0

        
        int64_t val_Rn = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t val_Rm = (Rm == 31) ? 0 : CURRENT_STATE.REGS[Rm];
        int64_t result = val_Rn - val_Rm;

        if (Rd != 31) {
            NEXT_STATE.REGS[Rd] = result;
        }

        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;       
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

        // Increment PC
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

    // SUBS: Xd, Xn, imm12 (immediate) and CMP: Xn, imm12 (immediate)
    else if ((instr >> 24) == 0xF1) {

        uint32_t imm12 = (instr >> 10) & 4095; // Bits 21-10
        uint32_t Rn = (instr >> 5) & 31;  // Bits 9-5
        uint32_t Rd = instr & 31;         // Bits 4-0

        // Shift case
        if (((instr >> 22) & 3) == 0x1) {
            imm12 = imm12 << 12;
        }

        int64_t val_Rn = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t result = val_Rn - imm12;

        if (Rd != 31) {
            NEXT_STATE.REGS[Rd] = result;
        }

        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;     
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

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

        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;       
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

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

        if (offset & 0x80000) {                       // Check negativity
            offset |= 0xFFF00000;
        }

        // Evaluate condition
        int branch = FALSE;
        switch (cond) {
            case 0x0: branch = (CURRENT_STATE.FLAG_Z == 1); break; // BEQ: Z == 1
            case 0x1: branch = (CURRENT_STATE.FLAG_Z == 0); break; // BNE: Z == 0
            case 0xA: branch = (CURRENT_STATE.FLAG_N == 0); break; // BGE: N == V (V=0, so N == 0)
            case 0xB: branch = (CURRENT_STATE.FLAG_N != 0); break; // BLT: N != V (V=0, so N != 0)
            case 0xC: branch = (CURRENT_STATE.FLAG_Z == 0 && CURRENT_STATE.FLAG_N == 0); break; // BGT: Z == 0 && N == V (V=0, so Z == 0 && N == 0)
            case 0xD: branch = (CURRENT_STATE.FLAG_Z == 1 || CURRENT_STATE.FLAG_N != 0); break; // BLE: Z == 1 || N != V (V=0, so Z == 1 || N != 0)
            default: break; // Unsupported condition
        }

        // Set next PC
        NEXT_STATE.PC = branch ? (CURRENT_STATE.PC + offset) : (CURRENT_STATE.PC + 4);
    }

    // Branch to Register: BR Xn
    else if ((instr & 0xFFFFFC1F) == 0xD61F0000) {

        uint32_t Rn = (instr >> 5) & 31; // bits 9-5

        uint64_t target_addr = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];

        NEXT_STATE.PC = target_addr;
    }

    // LSL, LSR: Xd, Xn, imms, immr (immediate)
    else if ((instr >> 22) == 0x34D){

        uint32_t immr = (instr >> 16) & 0x3F; // bits 20-16
        uint32_t imms = (instr >> 10) & 0x3F; // bits 15-10
        uint32_t Rn = (instr >> 5) & 31; // bits 9-5
        uint32_t Rd = instr & 31; // bits 4-0
        
        int64_t Rn_val = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t result;

        if (imms == 0x3F) { // LSR
            result = Rn_val >> immr;
        }
        else{ // LSL
            result = Rn_val << (64 - immr);
        }

        if (Rd != 31){
            NEXT_STATE.REGS[Rd] = result;
        }

        NEXT_STATE.FLAG_N = (result < 0) ? 1 : 0;   
        NEXT_STATE.FLAG_Z = (result == 0) ? 1 : 0;

        // Increment PC
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;

    }
    // MOVZ: Xd, imm16
    else if ((instr >> 23) == 0x1A5)
    {
        uint32_t imm16 = (instr >> 5) & 0xFFFF; // bits 20-5
        uint32_t Rd = instr & 31; // bits 4-0

        uint64_t value = imm16;

        if (Rd != 31) {
            NEXT_STATE.REGS[Rd] = value;
        }

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

    else if (((instr >> 21) & 0x1FD) == 0x1C0) {

        uint32_t size = (instr >> 30) & 0x3; // bits 31-30
        uint32_t load_or_save = (instr >> 22) & 0x3; // bits 23-22, if  == 01: load, else: save
        int32_t imm9 = (instr >> 12) & 0x1FF; // bits 20-12
        uint32_t Rn = (instr >> 5) & 31; // bits 9-5
        uint32_t Rt = instr & 31; // bits 4-0
        
        if (imm9 & 0x100) {   // If bit 8 is 1, sign-extend to 32 bits
            imm9 |= 0xFFF80000;
        }

        uint64_t base_addr = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        uint64_t target_addr = base_addr + imm9;

        
        if (!load_or_save) { // Save case
            uint64_t value = (Rt == 31) ? 0 : CURRENT_STATE.REGS[Rt];
            if(size == 0x3) { // STUR
                uint32_t low = value & 0xFFFFFFFF;
                uint32_t high = (value >> 32) & 0xFFFFFFFF;
                mem_write_32(target_addr, low);
                mem_write_32(target_addr + 4, high);
            }
            else if (size == 0x0){ // STURB
                uint32_t bits_8 = value & 0xFF;
                mem_write_32(target_addr, bits_8);
            }
            else if (size == 0x1){ // STURH
                uint32_t bits_16 = value & 0xFFFF;
                mem_write_32(target_addr, bits_16);
            }
        }
        else { // Load case
            uint64_t value = 0;
            if(size == 0x3) { // LDUR
                uint32_t low = mem_read_32(target_addr);
                uint32_t high = mem_read_32(target_addr + 4);
                value = low | ((uint64_t)high << 32);
            }
            else if (size == 0x0){ // LDURB
                value = mem_read_32(target_addr) & 0xFF;
            }
            else if (size == 0x1){ // LDURH
                value = mem_read_32(target_addr) & 0xFFFF;
            }

            if(Rt != 31){
                NEXT_STATE.REGS[Rt] = value;
            }
        }
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

    // ADD: Xd, Xn, imm (immediate)
    else if ((instr >> 23) == 0x122) {

        uint32_t imm12 = (instr >> 10) & 4095; // Bits 21-10
        uint32_t Rn = (instr >> 5) & 31;  // Bits 9-5
        uint32_t Rd = instr & 31;         // Bits 4-0

        // Shift case
        if (((instr >> 22) & 1) == 0x1) {
            imm12 = imm12 << 12;
        }
        
        int64_t val_Rn = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t result = val_Rn + imm12;

        if (Rd != 31) {
            NEXT_STATE.REGS[Rd] = result;
        }

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
    
    // ADD: Xd, Xn, Xm (extended register)
    else if ((instr >> 21) == 0x459) {

        uint32_t Rm = (instr >> 16) & 31; // Bits 20-16
        uint32_t Rn = (instr >> 5) & 31;  // Bits 9-5
        uint32_t Rd = instr & 31;         // Bits 4-0
        
        int64_t val_Rn = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t val_Rm = (Rm == 31) ? 0 : CURRENT_STATE.REGS[Rm];
        int64_t result = val_Rn + val_Rm;

        if (Rd != 31) {
            NEXT_STATE.REGS[Rd] = result;
        }

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

    // MUL: Xd, Xn, Xm 
    else if ((instr >> 21) == 0x4D8) {

        uint32_t Rm = (instr >> 16) & 31; // Bits 20-16
        uint32_t Rn = (instr >> 5) & 31;  // Bits 9-5
        uint32_t Rd = instr & 31;         // Bits 4-0
        
        int64_t val_Rn = (Rn == 31) ? 0 : CURRENT_STATE.REGS[Rn];
        int64_t val_Rm = (Rm == 31) ? 0 : CURRENT_STATE.REGS[Rm];
        int64_t result = val_Rn * val_Rm;

        if (Rd != 31) {
            NEXT_STATE.REGS[Rd] = result;
        }

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }

    // CBZ: Xt, label
    else if ((instr >> 24) == 0xB4) {

        int32_t imm19 = (instr >> 5) & 0x7FFFF; // bits 23-5
        uint32_t Rt = instr & 31;               // bits 4-0

        if (imm19 & 0x40000) { // If bit 18 is 1, sign-extend to 32 bits
            imm19 |= 0xFFF80000;
        }

        int64_t offset = (int64_t)imm19 << 2;

        int64_t val_Rt = (Rt == 31) ? 0 : CURRENT_STATE.REGS[Rt];

        // Branch if Rt == 0
        if (val_Rt == 0) {
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        } else {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        }
    }
    
    // CBNZ: Xt, label
    else if ((instr >> 24) == 0xB5) {

        int32_t imm19 = (instr >> 5) & 0x7FFFF; // bits 23-5
        uint32_t Rt = instr & 31;               // bits 4-0

        if (imm19 & 0x40000) { // If bit 18 is 1, sign-extend to 32 bits
            imm19 |= 0xFFF80000;
        }

        int64_t offset = (int64_t)imm19 << 2;

        int64_t val_Rt = (Rt == 31) ? 0 : CURRENT_STATE.REGS[Rt];

        // Branch if Rt != 0
        if (val_Rt != 0) {
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        } else {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        }
    }

    // Unimplemented instruction: halt simulator
    else {
        printf("Unimplemented instruction: 0x%08x at PC: 0x%" PRIx64 "\n", instr, CURRENT_STATE.PC);
        RUN_BIT = FALSE;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

