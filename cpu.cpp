#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include "cpu.h"
#include "opcodes.h"
#include "memory.h"
#include "ppu.h"

namespace CPU {

typedef Addr_Mode AM;

void tick() {
	PPU::tick();
	PPU::tick();
	PPU::tick();
}

/* This code is a huge mess */
void print_opcode_bytes(uint16_t addr) {
	using namespace std;
	const auto inst = Instruction(mem[addr]);
	typedef Addr_Mode AM;
	int bytes;
	switch(inst.mode) {
		case AM::None:
		case AM::Accumulator:
		case AM::Implied: bytes = 1; break;
		case AM::Absolute:
		case AM::Absolute_X:
		case AM::Absolute_Y: bytes = 3; break;
		default: bytes = 2; break;
	}
	for(int i = 0; i < 3; i++) {
		if(i < bytes)
			cout << setfill('0') << setw(2) << uppercase << hex
			     << int(mem[addr+i]);
		else
			cout << "  ";
		cout << " ";
	}
	cout << " ";
}

void print_inst_operand(uint16_t addr) {
	using namespace std;
	stringstream ss;
	const auto inst = Instruction(mem[addr]);
	typedef Addr_Mode AM;
	switch(inst.mode) {
		case AM::None:
		case AM::Accumulator:
		case AM::Implied: break;
		case AM::Absolute:
		case AM::Absolute_X:
		case AM::Absolute_Y:
		case AM::Indirect:
			ss << "$" << uppercase << hex << setfill('0') << setw(2)
			   << int(mem[addr+2]) << int(mem[addr+1]); break;
		case AM::Immediate:
			ss << "#$" << uppercase << setfill('0') << setw(2)
			   << hex << int(mem[addr+1]); break;
		case AM::Relative:
			ss << "$" << uppercase << hex << setfill('0') << setw(4)
			   << (addr + int8_t(mem[addr+1]) + 2); break;
		default:
			ss << "$" << uppercase << setfill('0') << setw(2)
			   << hex << int(mem[addr+1]); break;
		//default: break;
	}
	ss << "     " << getString(inst.mode);
	cout << setfill(' ') << setw(28) << left << ss.str();
}

void print_regs(const Registers& reg) {
	std::cout << std::setfill('0') << std::right
	    << "A:"  << std::hex << std::setw(2) << int(reg.A) << " "
	    << "X:"  << std::hex << std::setw(2) << int(reg.X) << " "
	    << "Y:"  << std::hex << std::setw(2) << int(reg.Y) << " "
	    << "P:"  << std::hex << std::setw(2) << int(reg.P) << " "
	    << "SP:" << std::hex << std::setw(2) << int(reg.S) << " ";
	std::cout << "CYC:" << std::dec << std::right << std::setw(3)
		<< std::setfill(' ') << reg.cycle << " ";
	std::cout << "SL:" << reg.scanline;
}

/* cycle-accurate mini-instructions for reads, writes, or read-writes
   for various addressing modes.
   See  http://nesdev.com/6502_cpu.txt for details

   NOTE: there is some code duplication (sorry!) so if you find a bug in a write
   then check the corresponding read and modify as well, etc.

   Also several of the instructions have spurious reads or writes
   keeping these is important because reading or writing to a certain
   address might do something funky
*/

template <> uint8_t read<AM::Accumulator>(Registers& reg) { 
	auto res = mem[reg.PC]; CPU::tick();
	return 0;
}
template <> uint8_t read<AM::Implied>(Registers& reg) {
	auto res = mem[reg.PC]; CPU::tick();
	return 0;
}
template <> uint8_t read<AM::Immediate>(Registers& reg) {
	auto res = mem[reg.PC++]; CPU::tick();
	return res;
}
template <> uint8_t read<AM::Absolute>(Registers& reg) {
	uint8_t low  = mem[reg.PC++];       CPU::tick();
	uint8_t high = mem[reg.PC++];       CPU::tick();
	uint8_t res = mem[low | high << 8]; CPU::tick();
	//std::cout << "Read " << std::hex << int(res) << " from "
	//          << std::hex << uint16_t(low | high << 8) << "\n";
	return res;
}
template <> uint8_t read<AM::Zero_Page>(Registers& reg) {
	uint8_t addr = mem[reg.PC++]; CPU::tick();
	auto res = mem[addr];         CPU::tick();
	return res;
}
uint8_t read_zp_index(Registers& reg, uint8_t index) {
	uint8_t addr = mem[reg.PC++]; CPU::tick();
	mem[addr];
	addr += index;                CPU::tick();
	auto res = mem[addr];         CPU::tick();
	return res;
}		
template <> uint8_t read<AM::Zero_Page_X>(Registers& reg) {
	return read_zp_index(reg, reg.X);
}
template <> uint8_t read<AM::Zero_Page_Y>(Registers& reg) {
	return read_zp_index(reg, reg.Y);
}
uint8_t read_abs_index(Registers& reg, uint8_t index) {
	uint8_t low  = mem[reg.PC++];                 CPU::tick();
	uint8_t high = mem[reg.PC++];
	uint16_t delta = low + index;                 CPU::tick();
	auto res = mem[uint8_t(delta) | high << 8];   CPU::tick();
	if(delta > 0xFF) {
		res = mem[uint16_t(delta + (high << 8))]; CPU::tick();
	}
	return res;
}	
template <> uint8_t read<AM::Absolute_X>(Registers& reg) {
	return read_abs_index(reg, reg.X);
}	
template <> uint8_t read<AM::Absolute_Y>(Registers& reg) {
	return read_abs_index(reg, reg.Y);
}
template <> uint8_t read<AM::Indirect_X>(Registers& reg) {
	uint8_t addr1 = mem[reg.PC++];  CPU::tick();
	mem[addr1];
	addr1 += reg.X;                          CPU::tick();
	uint16_t addr2 = mem[addr1];             CPU::tick();
	addr2 |= (mem[uint8_t(addr1 + 1)] << 8); CPU::tick();
	auto res = mem[addr2];                   CPU::tick();
	return res;	
}

template <> uint8_t read<AM::Indirect_Y>(Registers& reg) {
	auto addr = mem[reg.PC++];                    CPU::tick();
	uint8_t low  = mem[addr];                     CPU::tick();
	uint8_t high = mem[uint8_t(addr+1)];
	uint16_t delta = low + reg.Y;                 CPU::tick();
	auto res = mem[uint8_t(delta) | high << 8];   CPU::tick();
	if(delta > 0xFF) {
		res = mem[uint16_t(delta + (high << 8))]; CPU::tick();
	}
	return res;
}

//writes
template <Addr_Mode mode> void write(Registers& reg, uint8_t data) {
	assert(false);
}
template <> void write<AM::Absolute>(Registers& reg, uint8_t data) {
	uint16_t addr = mem[reg.PC++]; CPU::tick();
	addr |= (mem[reg.PC++] << 8);  CPU::tick();
	mem[addr] = data;              CPU::tick();
	return;
}

template <> void write<AM::Zero_Page>(Registers& reg, uint8_t data) {
	uint8_t addr = mem[reg.PC++]; CPU::tick();
	mem[addr] = data;             CPU::tick();
	return;
}
void write_zp_index(Registers& reg, uint8_t index, uint8_t data) {
	uint8_t addr = mem[reg.PC++]; CPU::tick();
	mem[addr];
	addr += index;                CPU::tick();
	mem[addr] = data;             CPU::tick();
	return;
}
template <> void write<AM::Zero_Page_X>(Registers& reg, uint8_t data) {
	write_zp_index(reg, reg.X, data);
}
template <> void write<AM::Zero_Page_Y>(Registers& reg, uint8_t data) {
	write_zp_index(reg, reg.Y, data);
}
void write_abs_index(Registers& reg, uint8_t index, uint8_t data) {
	uint8_t low    = mem[reg.PC++];            CPU::tick();
	uint16_t high  = mem[reg.PC++];
	uint16_t delta = low + index;              CPU::tick();
	mem[uint8_t(delta) | high << 8];           CPU::tick();
	mem[uint16_t(delta + (high << 8))] = data; CPU::tick();
	return;
}	
template <> void write<AM::Absolute_X>(Registers& reg, uint8_t data) {
	return write_abs_index(reg, reg.X, data);
}	
template <> void write<AM::Absolute_Y>(Registers& reg, uint8_t data) {
	return write_abs_index(reg, reg.Y, data);
}

template <> void write<AM::Indirect_X>(Registers& reg, uint8_t data) {
	uint8_t addr1 = mem[reg.PC++];  CPU::tick();
	mem[addr1];
	addr1 += reg.X;                          CPU::tick();
	uint16_t addr2 = mem[addr1];             CPU::tick();
	addr2 |= (mem[uint8_t(addr1 + 1)] << 8); CPU::tick();
	mem[addr2] = data;                       CPU::tick();
	return;
}

template <> void write<AM::Indirect_Y>(Registers& reg, uint8_t data) {
	auto addr = mem[reg.PC++];                 CPU::tick();
	uint8_t low  = mem[addr];                  CPU::tick();
	uint8_t high = mem[uint8_t(addr+1)];
	uint16_t delta = low + reg.Y;              CPU::tick();
	mem[uint8_t(delta) | high << 8];           CPU::tick();
	mem[uint16_t(delta + (high << 8))] = data; CPU::tick();
	return;
}

//read-writes
template <Addr_Mode mode> modPair modify(Registers& reg, modifyOp op) {
	assert(false);
	return {0,0};
}

template <> modPair modify<AM::Accumulator>(Registers& reg, modifyOp op) {
	auto old = reg.A;
	reg.A = op(reg, reg.A);
    CPU::tick();
	reg.setFlag(FLAG::ZERO, !reg.A);
	return {old,reg.A};
}

template <> modPair modify<AM::Absolute>(Registers& reg, modifyOp op) {
	uint8_t low  = mem[reg.PC++];        CPU::tick();
	uint8_t high = mem[reg.PC++];        CPU::tick();
	auto data = mem[low | high << 8];    CPU::tick();
	mem[low | high << 8] = data;         CPU::tick();
	mem[low | high << 8] = op(reg,data); CPU::tick();
	return {data,op(reg,data)};
}

template <> modPair modify<AM::Zero_Page>(Registers& reg, modifyOp op) {
	uint8_t addr = mem[reg.PC++]; CPU::tick();
	auto data = mem[addr];        CPU::tick();
	mem[addr] = data;             CPU::tick();
	mem[addr] = op(reg,data);     CPU::tick();
	return {data,op(reg,data)};
}
modPair modify_zp_index(Registers& reg, uint8_t index, modifyOp op) {
	uint8_t addr = mem[reg.PC++]; CPU::tick();
	mem[addr];
	addr += index;                CPU::tick();
	auto data = mem[addr];        CPU::tick();
	mem[addr] = data;             CPU::tick();
	mem[addr] = op(reg,data);     CPU::tick();
	return {data,op(reg,data)};
}		
template <> modPair modify<AM::Zero_Page_X>(Registers& reg, modifyOp op) {
	return modify_zp_index(reg, reg.X, op);
}
template <> modPair modify<AM::Zero_Page_Y>(Registers& reg, modifyOp op) {
	return modify_zp_index(reg, reg.Y, op);
}
modPair modify_abs_index(Registers& reg, uint8_t index, modifyOp op) {
	uint8_t low    = mem[reg.PC++];          CPU::tick();
	uint16_t high  = mem[reg.PC++];
	uint16_t delta = low + index;            CPU::tick();
	mem[uint8_t(delta) | high << 8];         CPU::tick();
	uint16_t addr = delta + (high << 8);
	auto data = mem[addr];                   CPU::tick();
	mem[addr] = data;                        CPU::tick();
	mem[addr] = op(reg,data);                CPU::tick();
	return {data,op(reg,data)};
}
template <> modPair modify<AM::Absolute_X>(Registers& reg, modifyOp op) {
	return modify_abs_index(reg, reg.X, op);
}
template <> modPair modify<AM::Absolute_Y>(Registers& reg, modifyOp op) {
	return modify_abs_index(reg, reg.Y, op);
}

template <> modPair modify<AM::Indirect_X>(Registers& reg, modifyOp op) {
	uint8_t addr1 = mem[reg.PC++];           CPU::tick();
	mem[addr1];
	addr1 += reg.X;                          CPU::tick();
	uint16_t addr2 = mem[addr1];             CPU::tick();
	addr2 |= (mem[uint8_t(addr1 + 1)] << 8); CPU::tick();
	uint8_t data = mem[addr2];               CPU::tick();
	mem[addr2] = data;                       CPU::tick();
	mem[addr2] = op(reg,data);               CPU::tick();
	return {data,op(reg,data)};
}
template <> modPair modify<AM::Indirect_Y>(Registers& reg, modifyOp op) {
	auto addr = mem[reg.PC++];            CPU::tick();
	uint8_t low  = mem[addr];             CPU::tick();
	uint8_t high = mem[uint8_t(addr+1)];
	uint16_t delta = low + reg.Y;         CPU::tick();
	mem[uint8_t(delta) | high << 8];      CPU::tick();
	uint16_t loc = delta + (high << 8);
	auto data = mem[loc];                 CPU::tick();
	mem[loc] = data;                      CPU::tick();
	mem[loc] = op(reg, data);             CPU::tick();
	return {data,op(reg,data)};
}

} //end namespace
