#include "alu.h"

void alu_reg_operand(int operation, int reg, struct Cpu * cpu)
{
    if (operation == 5)
    {
        // xor : result stored in register A
        cpu->registers[A] = cpu->registers[A] ^ cpu->registers[reg]; 
    }
}

void alu_immediate_operand()
{

}